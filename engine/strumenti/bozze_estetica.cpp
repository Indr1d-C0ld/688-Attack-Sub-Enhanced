// Bozze grafiche della rifinitura estetica (roadmap 33; dossier docs/ricerca/25_estetica_simulatori.md §8): le stesse
// schermate nelle tre varianti del linguaggio comune, fuori dal codice del gioco.
// - A «Lamiera di bordo»: la lingua di SELECT.CLS del 1989, pannelli avvitati, tasti a spia, testo inciso;
// - B «La base»: luoghi e documenti (scrivania, portablocco, cartelline);
// - C «Il quadro di comando»: consolle con schermi a fosforo e tasti funzione.
// Coi pezzi del kit (grafica/consolle, cornici, cerimonia) e i dati veri dell'inventario (docs/ricerca/schermate_prima).
// Scrive BMP a 640x480 nella cartella d'uscita; la carta del teatro della scelta la incolla tools/estetica/bozze.py dal
// ritaglio dell'istantanea, al rettangolo che questo programma stampa («CARTA nome x y w h»).
// Uso: bozze_estetica <dati> <cartella d'uscita>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

#include "grafica/cerimonia.hpp"
#include "grafica/consolle.hpp"
#include "grafica/cornici.hpp"
#include "grafica/font.hpp"
#include "grafica/lamiera.hpp"
#include "grafica/palette.hpp"
#include "grafica/tela.hpp"
#include "grafica/tema_stazioni.hpp"

using namespace m688;
namespace cer = m688::cerimonia;

namespace {

struct Font5 {
    Font orig, inc7, inc5, fos7, fos5, stamp;
};

constexpr std::uint8_t NERO = 0, GRIGIO = 7, GRIGIO_SCURO = 8, CIANO = 3, CIANO_CHIARO = 11, GIALLO = 14, BIANCO = 15,
                       ROSSO = 4, ROSSO_CHIARO = 12, VERDE_CHIARO = 10, BLU = 1;

std::uint8_t legno(int k) { return static_cast<std::uint8_t>(cer::LEGNO + std::clamp(k, 0, cer::N_LEGNO - 1)); }
std::uint8_t carta_i(int k) { return static_cast<std::uint8_t>(cer::CARTA + std::clamp(k, 0, cer::N_CARTA - 1)); }
std::uint8_t oro(int k) { return static_cast<std::uint8_t>(cer::ORO + std::clamp(k, 0, cer::N_ORO - 1)); }
std::uint8_t kaki(int k) { return static_cast<std::uint8_t>(cer::KAKI + std::clamp(k, 0, cer::N_KAKI - 1)); }
std::uint8_t inchiostro(int k) { return static_cast<std::uint8_t>(cer::SMALTO_BLU + std::clamp(k, 0, cer::N_SMALTO_BLU - 1)); }
std::uint8_t timbro_i(int k) { return static_cast<std::uint8_t>(cer::TIMBRO + std::clamp(k, 0, cer::N_TIMBRO - 1)); }

int sp(const Font& f) { return f.formato() == 2 ? f.spazio_predefinito() : 1; }
int largo(const Font& f, const std::string& s) { return f.larghezza(s, 1, sp(f), f.formato() != 2); }
void scrivi(Tela& t, const Font& f, int x, int y, const std::string& s, std::uint8_t c) { f.scrivi(t, x, y, s, c, 1, sp(f), f.formato() != 2); }
// le maiuscole piccole del font del 1989 (note, sottotitoli)
void piccolo(Tela& t, const Font& f, int x, int y, const std::string& s, std::uint8_t c) { f.scrivi(t, x, y, s, c, 1, sp(f), false); }
// il testo inciso del 1989: l'ombra a +1,+1, poi il colore
void incisa(Tela& t, const Font& f, int x, int y, const std::string& s, std::uint8_t c, std::uint8_t ombra = NERO) {
    scrivi(t, f, x + 1, y + 1, s, ombra);
    scrivi(t, f, x, y, s, c);
}
void centrata(Tela& t, const Font& f, Rettangolo r, const std::string& s, std::uint8_t c, bool ombra = false) {
    const int x = r.x + (r.w - largo(f, s)) / 2, y = r.y + (r.h - f.righe()) / 2;
    if (ombra) incisa(t, f, x, y, s, c);
    else scrivi(t, f, x, y, s, c);
}

// i pezzi di A vengono da grafica/lamiera (il modulo che il gioco usera')
void smusso(Tela& t, Rettangolo r, std::uint8_t chiaro, std::uint8_t scuro, int spessore = 1) { lamiera::smusso(t, r, chiaro, scuro, spessore); }
void lamiera_(Tela& t, Rettangolo r, int tono = 7, std::uint32_t seme = 1) { lamiera::lamiera(t, r, tono, seme); }
void pannello_incassato(Tela& t, Rettangolo r, int tono = 8) { lamiera::pannello(t, r, tono); }
void vetro(Tela& t, Rettangolo r) { lamiera::vetro(t, r); }
Rettangolo targa(Tela& t, const Font& f, int cx, int y, const std::string& s) { return lamiera::targa(t, f, cx, y, s); }

// il piano di legno: venatura lungo x con la retinatura, nodi rari
void legno_piano(Tela& t, Rettangolo r, int base = 3) {
    for (int y = r.y; y < r.y + r.h; ++y)
        for (int x = r.x; x < r.x + r.w; ++x) {
            // venature larghe che ondeggiano, una trama fine lungo la fibra, le assi ogni 96 pixel
            const double vena = std::sin(y * 0.16 + 2.2 * std::sin(x * 0.009 + y * 0.013)) * 0.5 + 0.5;
            const double v = 0.28 + 0.30 * vena + 0.12 * (cer::caso(x / 9, y / 2, 9) - 0.5) - (y % 96 == 0 ? 0.35 : 0.0);
            t.pixel(x, y, legno(base - 2 + cer::retina(v, x, y, 4)));
        }
}

// il portablocco: tavola di masonite, la molla di metallo in alto
void portablocco(Tela& t, Rettangolo r) {
    for (int y = r.y; y < r.y + r.h; ++y)
        for (int x = r.x; x < r.x + r.w; ++x) t.pixel(x, y, legno(1 + cer::retina(0.4 + 0.2 * cer::caso(x / 2, y, 4), x, y, 1)));
    smusso(t, r, legno(4), NERO);
    const Rettangolo m{r.x + r.w / 2 - 50, r.y - 4, 100, 18};
    t.rettangolo_pieno(m.x, m.y, m.w, m.h, smalto(8));
    smusso(t, m, smalto(11), smalto(3));
    t.rettangolo_pieno(m.x + 30, m.y + 4, 40, 6, smalto(4));
    smusso(t, {m.x + 30, m.y + 4, 40, 6}, smalto(2), smalto(10));
}

// la riga d'aiuto: sempre in grigio, su una fascia, nella stessa forma (principio 5)
void riga_aiuto(Tela& t, const Font& f, Rettangolo r, const std::string& s, std::uint8_t fondo, std::uint8_t testo) {
    t.rettangolo_pieno(r.x, r.y, r.w, r.h, fondo);
    scrivi(t, f, r.x + 6, r.y + (r.h - f.righe()) / 2, s, testo);
}

// i tre pulsanti del piede di A e C (tasti a spia); il predefinito ha il filo d'oro
void piede_spie(Tela& t, const Font& f, int y, const std::vector<std::pair<std::string, ColoreLampada>>& p, int predefinito) {
    std::vector<lamiera::Tasto> k;
    for (const auto& [testo, c] : p) k.push_back({testo, c});
    lamiera::piede(t, f, y - 22, "", k, predefinito);   // il piede comune senza la riga d'aiuto
}

// --- i dati veri (inventario del 27/10/2026) ---

struct Valore {
    std::string etichetta, valore;
    bool cambiato = false;
};
const char* const PARTI[] = {"NATO", "PATTO DI VARSAVIA", "NEUTRALI"};
std::vector<Valore> righe_parte(int p) {
    std::vector<Valore> v = {{"SITUAZIONE", "DELLA MISSIONE"}, {"ROE", "DELLA MISSIONE"}, {"LIVELLO DELL'IA", "DELLA MISSIONE"},
                             {"ALLERTA DI PARTENZA", "DELLA MISSIONE"}, {"CARICO", "DELLA MISSIONE"}};
    if (p == 1) v[0] = {"SITUAZIONE", "GUERRA", true};   // un valore cambiato, per far vedere il colore
    return v;
}
const std::vector<Valore> GLOBALI = {{"METEO", "DEL TEATRO E DELLA STAGIONE"}, {"NAVIGAZIONE", "PERFETTA"},
                                     {"SENSORI E STAZIONI", "MODERNI"}, {"SEME", "NUOVO A OGNI PARTITA"},
                                     {"DURATA MASSIMA", "120 MIN"}, {"ALL'ESITO DELLA MISSIONE", "SI FERMA"},
                                     {"TEMPO COMPRESSO AGLI EVENTI", "NON SI FERMA"}};
const char* const IA[] = {"DIFESA", "ARMI", "REAZIONI", "FORZE", "ESM", "SONARISTI"};
const char* const IA_NOTA[] = {"ESCHE, EVASIONE, RITIRATA", "CARICO, TMA, FILO", "AGLI ALLEATI", "ALLERTA, MESSAGGI, ZONE", "DELLE UNITA'", "DELL'IA"};

struct Missione {
    std::string nome, teatro;
    bool usa, alfa;
};
const std::vector<Missione> MISSIONI = {{"TORPEX '89", "FAROE", true, true},          {"SCROLLATEVELO DI DOSSO", "FAROE", true, true},
                                        {"OPERAZIONI SUBACQUEE", "BARENTS", false, true}, {"MUMAR CADAVER", "GIBILTERRA", true, false},
                                        {"FUGA", "GIBILTERRA", true, true},              {"GULASCH", "OTRANTO", true, false},
                                        {"PASSERELLA", "KUNASHIR", true, true},           {"FESTA A SORPRESA", "CELTICA", true, true},
                                        {"RITORNO A CASA", "CELTICA", true, true},        {"COLPISCI E FUGGI", "BALTICO", true, false}};
const std::vector<Valore> OPZIONI = {{"TAB", "STAZIONI MODERNE"}, {"[ ]", "CARTA D'EPOCA"}, {"W", "METEO: MARE 3, SERENO"},
                                     {"K", "NEMICI: NORMALE"},     {"S", "SONARISTA: MEDIO"},  {"N", "NAVIGAZIONE: PERFETTA"},
                                     {"M", "MUSICA: ADLIB"}};

// --- A: Lamiera di bordo ---

void regolazioni_A(Tela& t, const Font5& f) {
    lamiera_(t, {0, 0, 640, 480}, 7);
    targa(t, f.inc7, 320, 6, "SANDBOX: IL VETRINO DELL'IA");
    incisa(t, f.orig, 10, 10, "TORPEX '89", CIANO_CHIARO);
    incisa(t, f.orig, 548, 10, "SCENE1A", GRIGIO);
    incisa(t, f.orig, 16, 32, "NESSUN GIOCATORE: ANCHE IL BATTELLO E' COMANDATO DALL'IA, CON LE STESSE REGOLE DI TUTTI.", BIANCO);
    // le tre parti in colonna: un pannello ciascuna, targhetta, cinque righe col valore nel vetro
    for (int p = 0; p < 3; ++p) {
        const Rettangolo r{8 + p * 210, 46, 204, 160};
        pannello_incassato(t, r, 8);
        targhetta(t, {r.x + 12, r.y + 8, r.w - 24, 0}, PARTI[p], f.inc5);
        const auto v = righe_parte(p);
        for (int i = 0; i < 5; ++i) {
            const int y = r.y + 24 + i * 27;
            const bool scelta = p == 1 && i == 0;
            incisa(t, f.orig, r.x + 12, y, v[static_cast<std::size_t>(i)].etichetta, BIANCO);
            const Rettangolo w{r.x + 12, y + 11, r.w - 24 - 12, 14};
            vetro(t, w);
            scrivi(t, f.orig, w.x + 4, w.y + 3, v[static_cast<std::size_t>(i)].valore,
                   v[static_cast<std::size_t>(i)].cambiato ? CIANO_CHIARO : GRIGIO);
            lampada_rettangolare(t, {r.x + r.w - 20, y + 13, 8, 10}, ColoreLampada::ambra, scelta);
            if (scelta) smusso(t, {w.x - 1, w.y - 1, w.w + 2, w.h + 2}, GIALLO, GIALLO);
        }
    }
    // la partita
    {
        const Rettangolo r{8, 212, 440, 176};
        pannello_incassato(t, r, 8);
        targhetta(t, {r.x + 12, r.y + 8, 0, 0}, "LA PARTITA", f.inc5);
        for (std::size_t i = 0; i < GLOBALI.size(); ++i) {
            const int y = r.y + 26 + static_cast<int>(i) * 21;
            incisa(t, f.orig, r.x + 14, y + 3, GLOBALI[i].etichetta, BIANCO);
            const Rettangolo w{r.x + 212, y, r.w - 212 - 10, 14};
            vetro(t, w);
            scrivi(t, f.orig, w.x + 4, w.y + 3, GLOBALI[i].valore, GRIGIO);
        }
    }
    // l'IA: levette e spie, come un quadro interruttori
    {
        const Rettangolo r{454, 212, 178, 176};
        pannello_incassato(t, r, 8);
        targhetta(t, {r.x + 12, r.y + 8, 0, 0}, "PARTI DELL'IA", f.inc5);
        for (int i = 0; i < 6; ++i) {
            const int y = r.y + 30 + i * 24;
            levetta(t, r.x + 18, y + 6, true);
            lampada_rettangolare(t, {r.x + 30, y + 1, 10, 10}, ColoreLampada::verde, true);
            incisa(t, f.orig, r.x + 48, y - 1, IA[i], BIANCO);
            lamiera::secondaria(t, f.orig, r.x + 48, y + 10, IA_NOTA[i], true);
        }
    }
    lamiera::piede(t, f.orig, 392, "SU E GIU' SCELGONO LA RIGA, SINISTRA E DESTRA CAMBIANO IL VALORE. CIANO: CAMBIATO.",
                   {{"ESC  INDIETRO", ColoreLampada::rossa}, {"S  SERIE DI 10 SEMI", ColoreLampada::ambra}, {"INVIO  AVVIA IL VETRINO", ColoreLampada::verde}}, 2);
    lamiera_(t, {0, 446, 640, 34}, 6);
    incisa(t, f.orig, 10, 456, "VARIANTE A - LAMIERA DI BORDO", smalto(10), smalto(3));
}

void scelta_A(Tela& t, const Font5& f) {
    lamiera_(t, {0, 0, 640, 480}, 7);
    targa(t, f.inc7, 320, 6, "SCELTA DELLA MISSIONE");
    incisa(t, f.orig, 10, 10, "COMSUBLANT", oro(6));
    incisa(t, f.orig, 520, 10, "FLOTTA DEL NORD", ROSSO_CHIARO);
    const char* schede[] = {"CARRIERA", "MISSIONI ORIGINALI", "MISSIONE RAPIDA", "EDITOR"};
    for (int i = 0; i < 4; ++i) pulsante(t, {8 + i * 158, 30, 150, 16}, schede[i], ColoreLampada::ambra, i == 1, false, f.orig);
    // l'elenco
    const Rettangolo l{8, 52, 400, 336};
    pannello_incassato(t, l, 8);
    lamiera::secondaria(t, f.orig, l.x + 14, l.y + 8, "N. MISSIONE");
    incisa(t, f.orig, l.x + 300, l.y + 8, "688", BIANCO, BLU);
    incisa(t, f.orig, l.x + 344, l.y + 8, "ALFA", BIANCO, ROSSO);
    for (std::size_t i = 0; i < MISSIONI.size(); ++i) {
        const int y = l.y + 24 + static_cast<int>(i) * 30;
        const bool scelta = i == 0;
        if (scelta) {
            vetro(t, {l.x + 8, y - 3, l.w - 16, 28});
            lampada_rettangolare(t, {l.x + 10, y + 5, 6, 10}, ColoreLampada::ambra, true);
        }
        const std::string n = std::to_string((i + 1) % 10) + ".";
        incisa(t, f.orig, l.x + 20, y + 2, n, scelta ? GIALLO : BIANCO);
        incisa(t, f.orig, l.x + 40, y + 2, MISSIONI[i].nome, scelta ? GIALLO : BIANCO);
        if (scelta) scrivi(t, f.orig, l.x + 40, y + 13, MISSIONI[i].teatro, GRIGIO);
        else lamiera::secondaria(t, f.orig, l.x + 40, y + 13, MISSIONI[i].teatro);
        // i tasti a spia del 1989: lente blu per il 688, rossa per l'Alfa; dove manca, la casella vuota
        const auto spia = [&](int x, bool c, std::uint8_t col) { lamiera::spia(t, {x, y + 1, 20, 18}, c, col); };
        spia(l.x + 300, MISSIONI[i].usa, 9);
        spia(l.x + 348, MISSIONI[i].alfa, ROSSO_CHIARO);
    }
    // la missione scelta
    const Rettangolo d{414, 52, 218, 336};
    pannello_incassato(t, d, 8);
    targhetta(t, {d.x + 12, d.y + 8, 0, 0}, "MISSIONE SCELTA", f.inc5);
    incisa(t, f.orig, d.x + 12, d.y + 26, "TORPEX '89", GIALLO);
    lamiera::secondaria(t, f.orig, d.x + 12, d.y + 38, "US NAVY - LOS ANGELES");
    lamiera::secondaria(t, f.orig, d.x + 12, d.y + 48, "TEATRO: FAROE 1989");
    const Rettangolo c{d.x + 6, d.y + 60, 206, 112};
    vetro(t, {c.x - 1, c.y - 1, c.w + 2, c.h + 2});
    std::printf("CARTA scelta_A %d %d %d %d\n", c.x, c.y, c.w, c.h);
    targhetta(t, {d.x + 12, d.y + 180, 0, 0}, "OPZIONI DELLA PARTITA", f.inc5);
    for (std::size_t i = 0; i < OPZIONI.size(); ++i) {
        const int y = d.y + 198 + static_cast<int>(i) * 15;
        lamiera::tasto_tastiera(t, {d.x + 12, y, 26, 12}, OPZIONI[i].etichetta, f.orig);
        incisa(t, f.orig, d.x + 44, y + 2, OPZIONI[i].valore, BIANCO);
    }
    lamiera::piede(t, f.orig, 392, "F1 MUSEO   P CODICE   V VETRINO   C ALBO DEI COMANDANTI   R SCHEDA SUCCESSIVA",
                   {{"ESC  ESCI", ColoreLampada::rossa}, {"R  SCHEDA SUCCESSIVA", ColoreLampada::ambra}, {"INVIO  AL BRIEFING", ColoreLampada::verde}}, 2);
    lamiera_(t, {0, 446, 640, 34}, 6);
    incisa(t, f.orig, 10, 456, "VARIANTE A - LAMIERA DI BORDO", smalto(10), smalto(3));
}

// --- B: La base ---

// una riga del modulo: etichetta battuta a macchina, puntini, il valore a penna
void riga_modulo(Tela& t, const Font5& f, int x, int y, int w, const Valore& v, bool scelta) {
    if (scelta)   // l'evidenziatore giallo sulla riga, retinato
        for (int yy = y - 2; yy < y + 10; ++yy)
            for (int xx = x - 3; xx < x + w + 3; ++xx)
                if ((xx + yy) % 2 == 0) t.pixel(xx, yy, GIALLO);
    scrivi(t, f.orig, x, y, v.etichetta, NERO);
    const int a = x + largo(f.orig, v.etichetta) + 4, b = x + w - largo(f.orig, v.valore) - 4;
    for (int xx = a; xx < b; xx += 3) t.pixel(xx, y + 7, carta_i(1));
    scrivi(t, f.orig, x + w - largo(f.orig, v.valore), y, v.valore, v.cambiato ? ROSSO : inchiostro(0));
}

void piede_carta(Tela& t, const Font5& f, int y, const std::vector<std::string>& p, int predefinito) {
    const int n = static_cast<int>(p.size()), w = (624 - 8 * (n - 1)) / n;
    for (int i = 0; i < n; ++i) {
        const Rettangolo r{8 + i * (w + 8), y, w, 22};
        pulsante(t, f.orig, r, p[static_cast<std::size_t>(i)], i == predefinito);
        if (i == predefinito) smusso(t, {r.x - 2, r.y - 2, r.w + 4, r.h + 4}, oro(6), oro(2));
    }
}

void regolazioni_B(Tela& t, const Font5& f) {
    legno_piano(t, {0, 0, 640, 480}, 4);
    portablocco(t, {64, 12, 512, 404});
    const Rettangolo c{76, 22, 488, 386};
    carta(t, c, 3);
    timbro(t, f.orig, 190, 372, "ESERCITAZIONE", -8.0, 2, 5);
    scrivi(t, f.orig, c.x + 14, c.y + 12, "COMSUBLANT - MODULO 7-S", NERO);
    scrivi(t, f.orig, c.x + 14, c.y + 22, "ORDINE DI ESERCITAZIONE SENZA EQUIPAGGIO (VETRINO DELL'IA)", NERO);
    scrivi(t, f.orig, c.x + 14, c.y + 36, "MISSIONE: TORPEX '89 (SCENE1A)", NERO);
    t.linea_orizzontale(c.x + 12, c.x + c.w - 12, c.y + 48, carta_i(1));
    // le parti: una riga per voce, una colonna per parte, come un modulo a caselle
    const int col[] = {c.x + 196, c.x + 296, c.x + 396};
    scrivi(t, f.orig, c.x + 14, c.y + 56, "1. LE PARTI", NERO);
    const char* corti[] = {"NATO", "PATTO", "NEUTRALI"};
    for (int p = 0; p < 3; ++p) scrivi(t, f.orig, col[p], c.y + 56, corti[p], NERO);
    t.linea_orizzontale(c.x + 14, c.x + c.w - 14, c.y + 66, NERO);
    for (int i = 0; i < 5; ++i) {
        const int y = c.y + 72 + i * 13;
        if (i == 0)   // l'evidenziatore sulla casella scelta (Patto, situazione)
            for (int yy = y - 2; yy < y + 10; ++yy)
                for (int xx = col[1] - 4; xx < col[1] + 92; ++xx)
                    if ((xx + yy) % 2 == 0) t.pixel(xx, yy, GIALLO);
        scrivi(t, f.orig, c.x + 14, y, righe_parte(0)[static_cast<std::size_t>(i)].etichetta, NERO);
        for (int p = 0; p < 3; ++p) {
            const Valore v = righe_parte(p)[static_cast<std::size_t>(i)];
            scrivi(t, f.orig, col[p], y, v.valore == "DELLA MISSIONE" ? "MISSIONE" : v.valore, v.cambiato ? ROSSO : inchiostro(0));
        }
        for (int xx = c.x + 14; xx < c.x + c.w - 14; xx += 3) t.pixel(xx, y + 10, carta_i(2));
    }
    t.linea_orizzontale(c.x + 12, c.x + c.w - 12, c.y + 142, carta_i(1));
    scrivi(t, f.orig, c.x + 14, c.y + 150, "2. LA PARTITA", NERO);
    t.linea_orizzontale(c.x + 14, c.x + 280, c.y + 160, NERO);
    for (std::size_t i = 0; i < GLOBALI.size(); ++i) riga_modulo(t, f, c.x + 14, c.y + 166 + static_cast<int>(i) * 13, 272, GLOBALI[i], false);
    scrivi(t, f.orig, c.x + 300, c.y + 150, "3. PARTI DELL'IA ATTIVE", NERO);
    t.linea_orizzontale(c.x + 300, c.x + c.w - 14, c.y + 160, NERO);
    for (int i = 0; i < 6; ++i) {
        const int y = c.y + 166 + i * 13;
        smusso(t, {c.x + 300, y - 1, 9, 9}, NERO, NERO);
        scrivi(t, f.orig, c.x + 301, y - 1, "X", inchiostro(0));
        scrivi(t, f.orig, c.x + 314, y, IA[i], NERO);
    }
    t.linea_orizzontale(c.x + 12, c.x + c.w - 12, c.y + 264, carta_i(1));
    scrivi(t, f.orig, c.x + 14, c.y + 272, "NOTE: NESSUN GIOCATORE. ANCHE IL BATTELLO E' COMANDATO DALL'IA,", NERO);
    scrivi(t, f.orig, c.x + 14, c.y + 282, "CON LE STESSE REGOLE DI TUTTI. NON VALE PER LA CARRIERA.", NERO);
    scrivi(t, f.orig, c.x + 14, c.y + 300, "VALORI A PENNA: BLU COME DA MISSIONE, ROSSO CAMBIATO.", carta_i(0));
    scrivi(t, f.orig, c.x + 300, c.y + 340, "FIRMA DEL COMANDANTE", carta_i(0));
    t.linea_orizzontale(c.x + 300, c.x + c.w - 20, c.y + 336, NERO);
    riga_aiuto(t, f.orig, {8, 422, 624, 13}, "SU E GIU' SCELGONO LA RIGA, SINISTRA E DESTRA CAMBIANO IL VALORE", legno(0), carta_i(4));
    piede_carta(t, f, 440, {"ESC  INDIETRO", "S  SERIE DI 10 SEMI", "INVIO  AVVIA IL VETRINO"}, 2);
    scrivi(t, f.orig, 10, 468, "VARIANTE B - LA BASE", carta_i(4));
}

void scelta_B(Tela& t, const Font5& f) {
    legno_piano(t, {0, 0, 640, 480}, 4);
    // la cartellina aperta: due fogli manila, le linguette delle schede in alto
    const char* schede[] = {"CARRIERA", "ORIGINALI", "RAPIDE", "EDITOR"};
    for (int i = 0; i < 4; ++i) {
        const Rettangolo r{24 + i * 104, 14, 96, 20};
        for (int y = r.y; y < r.y + r.h; ++y)
            for (int x = r.x + 4; x < r.x + r.w - 4; ++x) t.pixel(x, y, kaki(i == 1 ? 6 : 4));
        centrata(t, f.orig, r, schede[i], i == 1 ? NERO : kaki(1));
    }
    const Rettangolo cart{12, 32, 616, 362};
    for (int y = cart.y; y < cart.y + cart.h; ++y)
        for (int x = cart.x; x < cart.x + cart.w; ++x) t.pixel(x, y, kaki(5 + cer::retina(0.3 + 0.3 * cer::caso(x / 3, y, 2), x, y, 1)));
    smusso(t, cart, kaki(7), kaki(1));
    t.linea_verticale(cart.x + cart.w / 2, cart.y + 2, cart.y + cart.h - 3, kaki(2));   // la piega
    // il foglio sinistro: l'elenco battuto a macchina
    const Rettangolo s{24, 42, 288, 342};
    carta(t, s, 7);
    scrivi(t, f.orig, s.x + 12, s.y + 10, "MISSIONI ORIGINALI - ELENCO", NERO);
    t.linea_orizzontale(s.x + 10, s.x + s.w - 10, s.y + 20, NERO);
    for (std::size_t i = 0; i < MISSIONI.size(); ++i) {
        const int y = s.y + 28 + static_cast<int>(i) * 31;
        if (i == 0)
            for (int yy = y - 3; yy < y + 22; ++yy)
                for (int xx = s.x + 6; xx < s.x + s.w - 6; ++xx)
                    if ((xx + yy) % 2 == 0) t.pixel(xx, yy, GIALLO);
        scrivi(t, f.orig, s.x + 12, y, std::to_string((i + 1) % 10) + ". " + MISSIONI[i].nome, NERO);
        scrivi(t, f.orig, s.x + 28, y + 11, MISSIONI[i].teatro, carta_i(0));
        // i timbrini della parte: blu 688, rosso Alfa
        if (MISSIONI[i].usa) { smusso(t, {s.x + 200, y, 30, 12}, inchiostro(1), inchiostro(1)); piccolo(t, f.orig, s.x + 205, y + 3, "688", inchiostro(1)); }
        if (MISSIONI[i].alfa) { smusso(t, {s.x + 236, y, 34, 12}, timbro_i(1), timbro_i(1)); piccolo(t, f.orig, s.x + 240, y + 3, "ALFA", timbro_i(1)); }
    }
    // il foglio destro: la scheda della missione con la foto della carta fermata da una graffetta
    const Rettangolo d{328, 42, 288, 342};
    carta(t, d, 8);
    timbro(t, f.orig, d.x + 214, d.y + 316, "SECRET", 6.0, 2, 3);
    scrivi(t, f.orig, d.x + 12, d.y + 10, "MISSIONE 1: TORPEX '89", NERO);
    scrivi(t, f.orig, d.x + 12, d.y + 22, "US NAVY - LOS ANGELES (SSN-688)", carta_i(0));
    scrivi(t, f.orig, d.x + 12, d.y + 32, "TEATRO: FAROE 1989", carta_i(0));
    const Rettangolo c{d.x + 40, d.y + 50, 206, 112};
    t.rettangolo_pieno(c.x - 4, c.y - 4, c.w + 8, c.h + 14, BIANCO);   // il bordo della fotografia
    smusso(t, {c.x - 4, c.y - 4, c.w + 8, c.h + 14}, BIANCO, carta_i(1));
    std::printf("CARTA scelta_B %d %d %d %d\n", c.x, c.y, c.w, c.h);
    for (int i = 0; i < 14; ++i) t.pixel(c.x + 20 + i / 4, c.y - 8 + i, smalto(9)), t.pixel(c.x + 24 + i / 4, c.y - 8 + i, smalto(5));   // graffetta
    scrivi(t, f.orig, d.x + 12, d.y + 178, "OPZIONI DELLA PARTITA", NERO);
    t.linea_orizzontale(d.x + 12, d.x + d.w - 12, d.y + 188, NERO);
    for (std::size_t i = 0; i < OPZIONI.size(); ++i) {
        const int y = d.y + 194 + static_cast<int>(i) * 13;
        scrivi(t, f.orig, d.x + 12, y, OPZIONI[i].etichetta, carta_i(0));
        scrivi(t, f.orig, d.x + 50, y, OPZIONI[i].valore, inchiostro(0));
    }
    scrivi(t, f.orig, d.x + 12, d.y + 296, "F1 MUSEO  P CODICE  V VETRINO", carta_i(0));
    scrivi(t, f.orig, d.x + 12, d.y + 308, "C ALBO DEI COMANDANTI", carta_i(0));
    riga_aiuto(t, f.orig, {8, 402, 624, 13}, "FRECCE O CIFRE: LA MISSIONE   SINISTRA E DESTRA: IL BATTELLO   R: LA SCHEDA DOPO", legno(0), carta_i(4));
    piede_carta(t, f, 422, {"ESC  ESCI", "R  SCHEDA SUCCESSIVA", "INVIO  AL BRIEFING"}, 2);
    scrivi(t, f.orig, 10, 462, "VARIANTE B - LA BASE", carta_i(4));
}

// --- C: Il quadro di comando ---

void riga_fosforo(Tela& t, const Font& f, int x, int y, int w, const Valore& v, bool scelta) {
    if (scelta) {   // la riga scelta in negativo, come un cursore di terminale
        t.rettangolo_pieno(x - 2, y - 1, w + 4, f.righe() + 2, fosforo(5));
        scrivi(t, f, x, y, v.etichetta, fosforo(0));
        scrivi(t, f, x + w - largo(f, v.valore), y, v.valore, fosforo(0));
        return;
    }
    scrivi(t, f, x, y, v.etichetta, fosforo(3));
    if (v.cambiato) testo_fosforo(t, x + w - largo(f, v.valore), y, v.valore, f);
    else scrivi(t, f, x + w - largo(f, v.valore), y, v.valore, fosforo(4));
}

void testata_fosforo(Tela& t, const Font& f, Rettangolo v, const std::string& s, const std::string& destra) {
    t.rettangolo_pieno(v.x, v.y, v.w, f.righe() + 4, fosforo(2));
    scrivi(t, f, v.x + 4, v.y + 2, s, fosforo(7));
    scrivi(t, f, v.x + v.w - 4 - largo(f, destra), v.y + 2, destra, fosforo(7));
}

void consolle_cornice(Tela& t, const Font5& f, const std::string& nome) {
    pannello(t, {0, 0, 640, 480}, BattelloTema::usa688, 7);
    lamiera_(t, {3, 3, 634, 474}, 7, 11);
    targhetta(t, {320 - (largo(f.inc7, nome) + 16) / 2, 4, 0, 0}, nome, f.inc7);
}

void piede_tasti(Tela& t, const Font5& f, int y, const std::vector<std::pair<std::string, ColoreLampada>>& p, int predefinito) {
    piede_spie(t, f.orig, y, p, predefinito);
}

void regolazioni_C(Tela& t, const Font5& f) {
    consolle_cornice(t, f, "SALA ANALISI - VETRINO DELL'IA");
    const Rettangolo v = crt_rettangolare(t, {8, 22, 624, 380});
    const Font& g = f.fos5;
    testata_fosforo(t, g, v, "REGOLAZIONI DEL VETRINO", "TORPEX '89  SCENE1A");
    scrivi(t, g, v.x + 4, v.y + 14, "NESSUN GIOCATORE: ANCHE IL BATTELLO E' COMANDATO DALL'IA", fosforo(3));
    {   // le parti: una riga per voce, una colonna per parte
        const int col[] = {v.x + 200, v.x + 340, v.x + 480};
        testo_fosforo(t, v.x + 6, v.y + 30, "LE PARTI", g);
        for (int p = 0; p < 3; ++p) testo_fosforo(t, col[p], v.y + 30, PARTI[p], g);
        t.linea_orizzontale(v.x + 6, v.x + v.w - 8, v.y + 30 + g.righe() + 1, fosforo(4));
        for (int i = 0; i < 5; ++i) {
            const int y = v.y + 42 + i * 11;
            scrivi(t, g, v.x + 6, y, righe_parte(0)[static_cast<std::size_t>(i)].etichetta, fosforo(3));
            for (int p = 0; p < 3; ++p) {
                const Valore w = righe_parte(p)[static_cast<std::size_t>(i)];
                if (p == 1 && i == 0) {   // la casella scelta in negativo, come il cursore di un terminale
                    t.rettangolo_pieno(col[p] - 2, y - 1, 120, g.righe() + 2, fosforo(5));
                    scrivi(t, g, col[p], y, w.valore, fosforo(0));
                } else if (w.cambiato) testo_fosforo(t, col[p], y, w.valore, g);
                else scrivi(t, g, col[p], y, w.valore, fosforo(4));
            }
        }
    }
    testo_fosforo(t, v.x + 6, v.y + 112, "LA PARTITA", g);
    t.linea_orizzontale(v.x + 6, v.x + 360, v.y + 112 + g.righe() + 1, fosforo(4));
    for (std::size_t i = 0; i < GLOBALI.size(); ++i) riga_fosforo(t, g, v.x + 6, v.y + 124 + static_cast<int>(i) * 11, 354, GLOBALI[i], false);
    testo_fosforo(t, v.x + 400, v.y + 112, "PARTI DELL'IA", g);
    t.linea_orizzontale(v.x + 400, v.x + v.w - 8, v.y + 112 + g.righe() + 1, fosforo(4));
    for (int i = 0; i < 6; ++i) {
        const int y = v.y + 124 + i * 11;
        t.rettangolo_pieno(v.x + 400, y, 18, g.righe(), fosforo(5));
        scrivi(t, g, v.x + 402, y, "ON", fosforo(0));
        scrivi(t, g, v.x + 424, y, IA[i], fosforo(4));
    }
    // il riquadro in basso: che cosa fa la riga scelta (lo spazio progettato)
    const int yb = v.y + 214;
    t.linea_orizzontale(v.x + 4, v.x + v.w - 5, yb, fosforo(3));
    testo_fosforo(t, v.x + 6, yb + 6, "PATTO DI VARSAVIA: SITUAZIONE", g);
    scrivi(t, g, v.x + 6, yb + 18, "LA SITUAZIONE DI PARTENZA FRA LA PARTE E LE ALTRE: DELLA MISSIONE,", fosforo(4));
    scrivi(t, g, v.x + 6, yb + 28, "PACE, CRISI O GUERRA. ORA: GUERRA (CAMBIATA).", fosforo(4));
    scrivi(t, g, v.x + 6, yb + 46, "VALORI LUMINOSI: CAMBIATI. TENUI: COME DA MISSIONE.", fosforo(3));
    scrivi(t, g, v.x + 6, v.y + v.h - 12, "SU/GIU' LA RIGA   SINISTRA/DESTRA IL VALORE", fosforo(3));
    piede_tasti(t, f, 410, {{"ESC INDIETRO", ColoreLampada::rossa}, {"S SERIE DI 10 SEMI", ColoreLampada::ambra}, {"INVIO AVVIA IL VETRINO", ColoreLampada::verde}}, 2);
    incisa(t, f.orig, 12, 450, "VARIANTE C - IL QUADRO DI COMANDO", smalto(10), smalto(3));
}

void scelta_C(Tela& t, const Font5& f) {
    consolle_cornice(t, f, "SALA OPERAZIONI - SCELTA DELLA MISSIONE");
    const Font& g = f.fos5;
    const Rettangolo a = crt_rettangolare(t, {8, 22, 316, 380});
    testata_fosforo(t, g, a, "MISSIONI ORIGINALI", "688  ALFA");
    for (std::size_t i = 0; i < MISSIONI.size(); ++i) {
        const int y = a.y + 18 + static_cast<int>(i) * 32;
        const bool scelta = i == 0;
        if (scelta) t.rettangolo_pieno(a.x + 2, y - 2, a.w - 4, 24, fosforo(5));
        const std::uint8_t c1 = scelta ? fosforo(0) : fosforo(6), c2 = scelta ? fosforo(1) : fosforo(3);
        scrivi(t, g, a.x + 6, y, std::to_string((i + 1) % 10) + " " + MISSIONI[i].nome, c1);
        scrivi(t, g, a.x + 18, y + 10, MISSIONI[i].teatro, c2);
        scrivi(t, g, a.x + a.w - 64, y, MISSIONI[i].usa ? "[X]" : "[ ]", c1);
        scrivi(t, g, a.x + a.w - 30, y, MISSIONI[i].alfa ? "[X]" : "[ ]", c1);
    }
    const Rettangolo b = crt_rettangolare(t, {328, 22, 304, 380});
    testata_fosforo(t, g, b, "MISSIONE 1", "TORPEX '89");
    scrivi(t, g, b.x + 4, b.y + 16, "US NAVY - LOS ANGELES (SSN-688)", fosforo(4));
    scrivi(t, g, b.x + 4, b.y + 26, "TEATRO: FAROE 1989", fosforo(4));
    const Rettangolo c{b.x + (b.w - 206) / 2, b.y + 40, 206, 112};
    smusso(t, {c.x - 2, c.y - 2, c.w + 4, c.h + 4}, fosforo(4), fosforo(4));
    std::printf("CARTA scelta_C %d %d %d %d\n", c.x, c.y, c.w, c.h);
    testo_fosforo(t, b.x + 4, b.y + 162, "OPZIONI DELLA PARTITA", g);
    t.linea_orizzontale(b.x + 4, b.x + b.w - 5, b.y + 162 + g.righe() + 1, fosforo(4));
    for (std::size_t i = 0; i < OPZIONI.size(); ++i) {
        const int y = b.y + 174 + static_cast<int>(i) * 11;
        t.rettangolo_pieno(b.x + 4, y - 1, 22, g.righe() + 1, fosforo(3));
        scrivi(t, g, b.x + 6, y, OPZIONI[i].etichetta, fosforo(0));
        scrivi(t, g, b.x + 32, y, OPZIONI[i].valore, fosforo(5));
    }
    scrivi(t, g, b.x + 4, b.y + b.h - 22, "F1 MUSEO  P CODICE  V VETRINO", fosforo(3));
    scrivi(t, g, b.x + 4, b.y + b.h - 12, "C ALBO   R SCHEDA SUCCESSIVA", fosforo(3));
    piede_tasti(t, f, 410, {{"ESC ESCI", ColoreLampada::rossa}, {"R SCHEDA SUCCESSIVA", ColoreLampada::ambra}, {"INVIO AL BRIEFING", ColoreLampada::verde}}, 2);
    incisa(t, f.orig, 12, 450, "VARIANTE C - IL QUADRO DI COMANDO", smalto(10), smalto(3));
}

// la combinazione consigliata: gli strumenti negli schermi di C (le statistiche della sandbox, pagina 1, coi dati veri)
void statistiche_C(Tela& t, const Font5& f) {
    consolle_cornice(t, f, "SALA ANALISI - STATISTICHE DELLA SANDBOX");
    const Font& g = f.fos5;
    const Rettangolo a = crt_rettangolare(t, {8, 22, 380, 380});
    testata_fosforo(t, g, a, "PROVA S5 CONVOGLIO", "0:09:58");
    const char* linguette[] = {"1 RIEPILOGO", "2 SCOPERTE", "3 ARMI", "4 REAZIONI", "5 ESPOSIZ."};
    for (int i = 0; i < 5; ++i) {
        const int x = a.x + 4 + i * 72;
        if (i == 0) t.rettangolo_pieno(x - 2, a.y + 14, 70, g.righe() + 2, fosforo(5));
        scrivi(t, g, x, a.y + 15, linguette[i], i == 0 ? fosforo(0) : fosforo(3));
    }
    struct R { const char *n, *a, *b; };
    const R righe[] = {{"UNITA'", "11", "1"}, {"PERSE / INFLITTE", "0 / 0", "0 / 0"}, {"SCAMBIO", "0,0", "0,0"},
                       {"SCOPERTE FATTE / SUBITE", "3 / 10", "10 / 3"}, {"RITARDO MEDIO DI SCOPERTA", "0 S", "0 S"},
                       {"DISTANZA MEDIA DI SCOPERTA", "4,3 KM", "3,1 KM"}, {"LANCI / A SEGNO", "0 / 0", "2 / 1"},
                       {"ESCHE / EFFICACI", "2 / 0", "0 / 0"}, {"SCOPERTA - ALLERTA", "-", "274 S"},
                       {"SCOPERTA - INGAGGIO", "371 S", "32 S"}, {"TEMPO ALLA SOLUZIONE", "-", "273 S"},
                       {"ERRORE AL LANCIO", "-", "685 M"}};
    testo_fosforo(t, a.x + 6, a.y + 32, "PER PARTE", g);
    testo_fosforo(t, a.x + 230, a.y + 32, "NATO", g);
    testo_fosforo(t, a.x + 300, a.y + 32, "PATTO", g);
    t.linea_orizzontale(a.x + 4, a.x + a.w - 5, a.y + 32 + g.righe() + 1, fosforo(4));
    int y = a.y + 44;
    for (const auto& r : righe) {
        scrivi(t, g, a.x + 6, y, r.n, fosforo(4));
        scrivi(t, g, a.x + 230, y, r.a, fosforo(6));
        scrivi(t, g, a.x + 300, y, r.b, fosforo(6));
        y += 11;
    }
    t.linea_orizzontale(a.x + 4, a.x + a.w - 5, y + 2, fosforo(4));
    testo_fosforo(t, a.x + 6, y + 8, "PK PER ARMA (A SEGNO / LANCIATE)", g);
    scrivi(t, g, a.x + 6, y + 20, "TYPE53", fosforo(4));
    scrivi(t, g, a.x + 230, y + 20, "-", fosforo(6));
    scrivi(t, g, a.x + 300, y + 20, "1 / 2", fosforo(6));
    // il secondo schermo: lo spazio che oggi resta nero diventa un grafico
    const Rettangolo b = crt_rettangolare(t, {392, 22, 240, 380});
    testata_fosforo(t, g, b, "SCOPERTE", "PER PARTE");
    const auto barra = [&](int yy, const char* n, int fatte, int subite) {
        scrivi(t, g, b.x + 6, yy, n, fosforo(5));
        for (int k = 0; k < fatte; ++k) t.rettangolo_pieno(b.x + 60 + k * 14, yy + 1, 11, 7, fosforo(6));
        for (int k = 0; k < subite; ++k) t.rettangolo_pieno(b.x + 60 + k * 14, yy + 14, 11, 7, fosforo(3));
    };
    barra(b.y + 30, "NATO", 3, 10);
    barra(b.y + 66, "PATTO", 10, 3);
    scrivi(t, g, b.x + 6, b.y + 102, "PIENO: FATTE   TENUE: SUBITE", fosforo(3));
    t.linea_orizzontale(b.x + 4, b.x + b.w - 5, b.y + 120, fosforo(3));
    testo_fosforo(t, b.x + 6, b.y + 128, "ARMI", g);
    scrivi(t, g, b.x + 6, b.y + 142, "PATTO: 2 LANCI, 1 A SEGNO", fosforo(5));
    scrivi(t, g, b.x + 6, b.y + 154, "NATO: 2 ESCHE, NESSUNA EFFICACE", fosforo(5));
    scrivi(t, g, b.x + 6, b.y + b.h - 22, "CLIC SU UNA RIGA:", fosforo(3));
    scrivi(t, g, b.x + 6, b.y + b.h - 12, "IL REPLAY DA LI'", fosforo(3));
    piede_tasti(t, f, 410, {{"TAB PAGINA", ColoreLampada::ambra}, {"FRECCE SCORRONO", ColoreLampada::ambra}, {"ESC INDIETRO", ColoreLampada::verde}}, 2);
    incisa(t, f.orig, 12, 450, "COMBINAZIONE: GLI STRUMENTI COME SCHERMI DI C", smalto(10), smalto(3));
}

// --- i pezzi di A, uno per uno (grafica/lamiera), e il piede comune ---

void pezzi_A(Tela& t, const Font5& f) {
    lamiera_(t, {0, 0, 640, 480}, 7);
    targa(t, f.inc7, 320, 6, "I PEZZI DELLA LAMIERA DI BORDO");
    // i tasti a spia nei cinque stati
    {
        const Rettangolo r{8, 36, 624, 92};
        pannello_incassato(t, r, 8);
        targhetta(t, {r.x + 12, r.y + 8, 0, 0}, "TASTI A SPIA: GLI STATI (PRINCIPIO 8)", f.inc5);
        const struct { const char* testo; ColoreLampada c; lamiera::Stato s; bool pred; const char* nota; } k[] = {
            {"R  SCHEDA", ColoreLampada::ambra, lamiera::Stato::normale, false, "NORMALE"},
            {"R  SCHEDA", ColoreLampada::ambra, lamiera::Stato::evidenziato, false, "EVIDENZIATO"},
            {"R  SCHEDA", ColoreLampada::ambra, lamiera::Stato::premuto, false, "PREMUTO"},
            {"P  CODICE", ColoreLampada::ambra, lamiera::Stato::disabilitato, false, "DISABILITATO"},
            {"INVIO  AVVIA", ColoreLampada::verde, lamiera::Stato::normale, true, "PREDEFINITO"},
        };
        for (int i = 0; i < 5; ++i) {
            const Rettangolo b{r.x + 16 + i * 120, r.y + 32, 104, 26};
            lamiera::tasto(t, b, k[i].testo, k[i].c, k[i].s, k[i].pred, f.orig);
            incisa(t, f.orig, b.x + (b.w - largo(f.orig, k[i].nota)) / 2, b.y + 36, k[i].nota, BIANCO);
        }
    }
    // i tre livelli di testo e i colori dei valori
    {
        const Rettangolo r{8, 134, 308, 196};
        pannello_incassato(t, r, 8);
        targhetta(t, {r.x + 12, r.y + 8, 0, 0}, "TESTO: TRE LIVELLI (PRINCIPIO 2)", f.inc5);
        targa(t, f.inc7, r.x + r.w / 2, r.y + 26, "TITOLO");
        targhetta(t, {r.x + 14, r.y + 60, 0, 0}, "INTESTAZIONE DI GRUPPO", f.inc5);
        incisa(t, f.orig, r.x + 14, r.y + 80, "TESTO: SCALA 1, SU UN FONDO PIENO", BIANCO);
        const struct { const char* et; const char* val; std::uint8_t c; bool scelto; } v[] = {
            {"DI SERIE", "DELLA MISSIONE", GRIGIO, false},
            {"CAMBIATO", "GUERRA", CIANO_CHIARO, false},
            {"RIGA SCELTA", "REGOLARE", CIANO_CHIARO, true},
        };
        for (int i = 0; i < 3; ++i) {
            const int y = r.y + 100 + i * 30;
            incisa(t, f.orig, r.x + 14, y + 3, v[i].et, BIANCO);
            const Rettangolo w{r.x + 120, y, 150, 14};
            vetro(t, w);
            scrivi(t, f.orig, w.x + 4, w.y + 3, v[i].val, v[i].c);
            lampada_rettangolare(t, {r.x + r.w - 26, y + 2, 8, 10}, ColoreLampada::ambra, v[i].scelto);
            if (v[i].scelto) smusso(t, {w.x - 1, w.y - 1, w.w + 2, w.h + 2}, GIALLO, GIALLO);
        }
    }
    // i colori delle spie e delle marine
    {
        const Rettangolo r{324, 134, 308, 196};
        pannello_incassato(t, r, 8);
        targhetta(t, {r.x + 12, r.y + 8, 0, 0}, "IL COLORE DICE IL GRUPPO (PRINCIPIO 9)", f.inc5);
        const struct { ColoreLampada c; const char* nota; } sp[] = {
            {ColoreLampada::verde, "AVANTI: AVVIA, CONFERMA"},
            {ColoreLampada::rossa, "USCIRE, RIFIUTARE"},
            {ColoreLampada::ambra, "LE SCELTE: SCHEDE, OPZIONI"},
        };
        for (int i = 0; i < 3; ++i) {
            const int y = r.y + 32 + i * 24;
            lampada_rettangolare(t, {r.x + 16, y, 14, 14}, sp[i].c, true);
            incisa(t, f.orig, r.x + 40, y + 3, sp[i].nota, BIANCO);
        }
        incisa(t, f.orig, r.x + 16, r.y + 112, "IL TITOLO NEL COLORE DELLA MARINA:", BIANCO);
        incisa(t, f.orig, r.x + 16, r.y + 130, "COMSUBLANT", oro(6));
        incisa(t, f.orig, r.x + 120, r.y + 130, "FLOTTA DEL NORD", ROSSO_CHIARO);
        incisa(t, f.orig, r.x + 16, r.y + 152, "LE PARTI COME I SIMBOLI DELLA CARTA:", BIANCO);
        const std::uint8_t parti[] = {CIANO_CHIARO, 13, GIALLO, VERDE_CHIARO};
        const char* nomi[] = {"AMICO", "OSTILE", "IGNOTO", "NEUTRALE"};
        for (int i = 0; i < 4; ++i) incisa(t, f.orig, r.x + 16 + i * 72, r.y + 170, nomi[i], parti[i]);
    }
    // il piede comune, come in tutte le schermate
    incisa(t, f.orig, 12, 340, "IL PIEDE, UGUALE DAPPERTUTTO (PRINCIPIO 5): LA RIGA D'AIUTO GRIGIA E AL PIU' QUATTRO TASTI.", BIANCO);
    incisa(t, f.orig, 12, 352, "IL TASTO E' SCRITTO SUL PULSANTE; INDIETRO A SINISTRA, AVANTI A DESTRA COL FILO D'ORO.", BIANCO);
    lamiera::piede(t, f.orig, 384, "F1 MUSEO   P CODICE   V VETRINO   C ALBO DEI COMANDANTI",
                   {{"ESC  ESCI", ColoreLampada::rossa}, {"R  SCHEDA SUCCESSIVA", ColoreLampada::ambra}, {"INVIO  AL BRIEFING", ColoreLampada::verde}}, 2);
    lamiera_(t, {0, 446, 640, 34}, 6);
    incisa(t, f.orig, 10, 456, "VARIANTE A - I PEZZI (GRAFICA/LAMIERA)", smalto(10), smalto(3));
}

}  // namespace

int main(int argc, char** argv) {
    if (argc < 3) {
        std::fprintf(stderr, "uso: %s <dati> <cartella d'uscita>\n", argv[0]);
        return 2;
    }
    const std::string dati = argv[1], fuori = argv[2];
    Palette p = Palette::carica(dati + "/palette/giorno.pal");
    cer::imposta_palette(p, Marina::usn);
    applica_tema(p, TemaStazione{});
    Font5 f{Font::carica(dati + "/font/originale.fnt"), Font::carica(dati + "/font/incisione688_7.fnt"),
            Font::carica(dati + "/font/incisione688_5.fnt"), Font::carica(dati + "/font/fosforo688_7x9.fnt"),
            Font::carica(dati + "/font/fosforo688_5x7.fnt"), Font::carica(dati + "/font/stampino_urss_10.fnt")};
    const struct { const char* nome; void (*disegna)(Tela&, const Font5&); } tavole[] = {
        {"regolazioni_A", regolazioni_A}, {"regolazioni_B", regolazioni_B}, {"regolazioni_C", regolazioni_C},
        {"scelta_A", scelta_A},           {"scelta_B", scelta_B},           {"scelta_C", scelta_C},
        {"statistiche_C", statistiche_C}, {"pezzi_A", pezzi_A},
    };
    for (const auto& x : tavole) {
        Tela t(640, 480);
        t.riempi(0);
        x.disegna(t, f);
        t.salva_bmp(fuori + "/" + x.nome + ".bmp", p);
    }
    return 0;
}
