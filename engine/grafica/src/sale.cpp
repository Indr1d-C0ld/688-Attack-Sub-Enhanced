#include "grafica/sale.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>

#include "grafica/palette.hpp"
#include "grafica/periscopio.hpp"   // ALTEZZA_VISTA
#include "nucleo/geometria.hpp"

namespace m688 {

namespace {

bool e_alfa(const TemaStazione& t) { return t.battello == BattelloTema::alfa; }

int spazio(const Font& f) { return f.formato() == 2 ? f.spazio_predefinito() : 1; }

// scritta del pannello: bianca sul grigio del 688, nera sul crema dell'Alfa; centrata su cx
void etichetta(Tela& t, int cx, int y, const std::string& s, const Font& f, bool alfa) {
    const int w = f.larghezza(s, 1, spazio(f), false);
    f.scrivi(t, cx - w / 2, y, s, alfa ? static_cast<std::uint8_t>(colori::NERO) : smalto(11), 1, spazio(f), false);
}
void etichetta_sx(Tela& t, int x, int y, const std::string& s, const Font& f, bool alfa) {
    f.scrivi(t, x, y, s, alfa ? static_cast<std::uint8_t>(colori::NERO) : smalto(11), 1, spazio(f), false);
}

void area(std::vector<AreaPulto>* aree, Pulto p, Rettangolo r, const std::string& comando = "") {
    if (aree) aree->push_back({p, r, comando});
}

// le campane dell'Enhanced, dall'indietro tutta alla massima: inglese d'epoca (688) e sovietiche traslitterate (Alfa:
// polnyj nazad, srednij, malyj, stop, samyj malyj vperjod, malyj, srednij, polnyj, samyj polnyj)
constexpr const char* CAMPANE_688[9] = {"BK FULL", "BK 2/3", "BK 1/3", "STOP", "AH 1/3", "AH 2/3", "AH STD", "AH FULL", "FLANK"};
constexpr const char* CAMPANE_ALFA[9] = {"PN", "SN", "MN", "STOP", "SMV", "MV", "SV", "PV", "SPV"};

std::string formato(const char* f, double v) {
    char s[32];
    std::snprintf(s, sizeof s, f, v);
    return s;
}

}  // namespace

FontConsolle FontConsolle::carica(const std::string& dati, BattelloTema b) {
    const auto c = [&](const char* n) { return Font::carica(dati + "/font/" + n + ".fnt"); };
    if (b == BattelloTema::alfa)
        return {c("stampino_urss_7"), c("stampino_urss_7"), c("stampino_urss_10"), c("stampino_urss_10"), c("cifre_5x9"),
                c("stampino_urss_10")};
    return {c("incisione688_7"), c("incisione688_5"), c("fosforo688_5x7"), c("fosforo688_7x9"), c("cifre_5x9"),
            c("incisione688_7")};
}

// --- Manovra ---

void pannello_governo(Tela& t, Rettangolo r, const TemaStazione& tema, const FontConsolle& f, const DatiGoverno& d,
                      std::vector<AreaPulto>* aree) {
    const bool alfa = e_alfa(tema);
    const Rettangolo prima = t.ritaglio();
    t.imposta_ritaglio(r);
    const int H = r.h, top = r.y + 4 + f.targa.righe() + 3;   // sotto le targhette
    const StileQuadrante sq = alfa ? StileQuadrante::bianco_alfa : StileQuadrante::scuro_688;
    const int campana = std::clamp(d.campana, 0, 8);

    // A. strumenti di governo: bussola col comandato, timone, piani, quota, velocita'
    const Rettangolo a{r.x, r.y, 296, H};
    pannello(t, a, tema.battello, 7);
    targhetta(t, {a.x + 6, a.y + 4, 0, 0}, alfa ? "BOKSIT" : "SHIP CONTROL", f.targa);
    const int etich = f.piccolo.righe() + 1;
    const int rc = std::max(12, std::min(32, (r.y + H - top - etich - 4) / 2)), rs = std::max(9, rc * 2 / 3);
    const int cy = top + rc + 1;
    const int xb = a.x + 8 + rc;
    // numeri: lo stampino dell'Alfa e' piu' largo, la bussola ne scrive quattro; i quadranti piccoli solo se ci stanno
    quadrante(t, xb, cy, rc, d.rotta, {0, 360, 0, 360, 36, alfa || rc < 24 ? 4 : 12, 0.1}, sq, &f.piccolo, &d.rotta_ordinata);
    const int numeri_piccoli = rs >= (alfa ? 22 : 18) ? 2 : 0;
    etichetta(t, xb, cy + rc + 2, alfa ? "KURS" : "HEADING", f.piccolo, alfa);
    area(aree, Pulto::governo, {xb - rc, cy - rc, 2 * rc + 1, 2 * rc + 1}, "pilota_automatico");
    const int xt = xb + rc + 8 + rs, xp = xt + 2 * rs + 10;
    quadrante(t, xt, cy, rs, d.barra_gradi, {-35, 35, -120, 120, 14, numeri_piccoli, 1.0}, sq, &f.piccolo);
    etichetta(t, xt, cy + rs + 2, alfa ? "VERT RUL" : "RUDDER", f.piccolo, alfa);
    const char* timone[3] = {"timone_sinistra", "timone_centro", "timone_dritta"};
    for (int k = 0; k < 3; ++k) area(aree, Pulto::governo, {xt - rs + k * (2 * rs + 1) / 3, cy - rs, (2 * rs + 1) / 3, 2 * rs + 1}, timone[k]);
    quadrante(t, xp, cy, rs, d.piani_gradi, {-30, 30, -120, 120, 12, numeri_piccoli, 1.0}, sq, &f.piccolo);
    etichetta(t, xp, cy + rs + 2, alfa ? "GOR RUL" : "PLANES", f.piccolo, alfa);
    const char* piani[3] = {"barra_su", "barra_zero", "barra_giu"};
    for (int k = 0; k < 3; ++k) area(aree, Pulto::governo, {xp - rs, cy - rs + k * (2 * rs + 1) / 3, 2 * rs + 1, (2 * rs + 1) / 3}, piani[k]);
    // quota: la colonna graduata col comandato e il tamburo (piedi sul 688, metri sull'Alfa)
    const int xq = xp + rs + 10, hq = r.y + H - top - 6;
    barra(t, {xq + 4, top, 9, hq}, d.quota_m / std::max(d.quota_max_m, 1.0), sq,
          d.cambia_quota ? d.quota_ordinata_m / std::max(d.quota_max_m, 1.0) : -1.0);
    const int xn = xq + 20;
    const double q = alfa ? d.quota_m : d.quota_m / 0.3048;
    const double giro = q - std::floor(q);
    tamburo(t, xn, top + 1, formato("%04.0f", std::floor(q)), giro, f.tamburo);
    etichetta_sx(t, xn, top + f.tamburo.righe() + 5, alfa ? "GLUB M" : "DEPTH FT", f.piccolo, alfa);
    if (!alfa && d.quota_periscopica >= 0) {   // Enhanced: la quota periscopica, verde
        lampada_rettangolare(t, {xn + 42, top + 1, 12, 8}, ColoreLampada::verde, d.quota_periscopica > 0);
        etichetta_sx(t, xn + 57, top + 2, "PERI", f.piccolo, false);
    }
    if (!alfa && d.schiera >= 0) {   // Enhanced: la schiera trainata, ambra sul fondo o danneggiata, rossa persa
        lampada_rettangolare(t, {xn + 42, top + 13, 12, 8}, d.schiera >= 3 ? ColoreLampada::rossa : ColoreLampada::ambra, d.schiera > 0);
        etichetta_sx(t, xn + 57, top + 14, "ARRAY", f.piccolo, false);
    }
    const int yv = top + f.tamburo.righe() + 8 + etich;
    if (alfa) {   // l'Alfa: niente display, la velocita' sul tamburo
        tamburo(t, xn, yv, formato("%02.0f", std::min(99.0, std::fabs(d.velocita_kn))), 0.0, f.tamburo);
        etichetta_sx(t, xn, yv + f.tamburo.righe() + 4, "SKOR UZ", f.piccolo, alfa);
    } else {
        sette_segmenti(t, xn + 2, yv + 2, formato("%4.1f", d.velocita_kn), ColoreLampada::rossa, f.cifre);
        etichetta_sx(t, xn, yv + f.cifre.righe() + 6, "KTS", f.piccolo, alfa);
        lampada_rettangolare(t, {xn + 32, yv + f.cifre.righe() + 4, 16, 7}, ColoreLampada::ambra, d.cavita);
        etichetta_sx(t, xn + 50, yv + f.cifre.righe() + 6, "CAV", f.piccolo, alfa);
    }

    // B. telegrafo di macchina: una finestra per campana, dalla massima in alto all'indietro tutta in basso
    const Rettangolo b{r.x + 300, r.y, 96, H};
    pannello(t, b, tema.battello, 7);
    targhetta(t, {b.x + 6, b.y + 4, 0, 0}, alfa ? "TELEGRAF" : "ENGINE ORDER", f.targa);
    // una colonna di 9 righe se le scritte ci stanno, altrimenti due (avanti 8-4 a sinistra, 3-0 a destra)
    const int spazio_righe = r.y + H - top - 2;
    const bool due = spazio_righe / 9 < f.piccolo.righe() - 2;
    const int righe_col = due ? 5 : 9, larg_col = due ? (b.w - 4) / 2 : b.w - 4;
    const int rh = std::max(5, std::min(due ? 14 : 11, spazio_righe / righe_col));
    for (int c = 8; c >= 0; --c) {
        const int k = 8 - c, col = due && k >= righe_col ? 1 : 0;
        const int y = top + (k - col * righe_col) * rh, x = b.x + 2 + col * larg_col;
        const bool attiva = c == campana;
        if (alfa) lampada_tonda(t, x + 8, y + rh / 2, std::max(2, std::min(4, rh / 2 - 1)), c < 3 ? ColoreLampada::rossa : ColoreLampada::verde, attiva);
        else lampada_rettangolare(t, {x + 4, y + 1, due ? 8 : 12, rh - 2}, ColoreLampada::ambra, attiva);
        etichetta_sx(t, x + (due ? 15 : 22), y + (rh - f.piccolo.righe()) / 2 + 1, alfa ? CAMPANE_ALFA[c] : CAMPANE_688[c], f.piccolo, alfa);
        area(aree, Pulto::governo, {x, y, larg_col, rh}, c > campana ? "telegrafo_avanti" : c < campana ? "telegrafo_indietro" : "");
    }

    // C. casse: BCP del 688 (albero di Natale, compenso, soffiaggio coperto) o pulsanti ПОКС e modi di Boksit dell'Alfa
    const Rettangolo c{r.x + 400, r.y, r.w - 400, H};
    pannello(t, c, tema.battello, 7);
    if (!alfa) {
        targhetta(t, {c.x + 6, c.y + 4, 0, 0}, "BALLAST CONTROL", f.targa);
        std::vector<bool> ap = d.aperture;
        if (ap.empty()) ap.assign(16, false);
        const Rettangolo alb{c.x + 6, top, 96, r.y + H - top - etich - 4};
        albero_di_natale(t, alb, ap, 8);
        const bool aperta = std::find(ap.begin(), ap.end(), true) != ap.end();
        etichetta(t, alb.x + alb.w / 2, alb.y + alb.h + 2, "HULL OPENINGS", f.piccolo, false);
        area(aree, Pulto::bcp, alb);
        const int xc = c.x + 108;
        lampada_rettangolare(t, {xc, top, 34, 9}, ColoreLampada::rossa, aperta);
        etichetta_sx(t, xc + 37, top + 1, "HULL OPEN", f.piccolo, false);
        const int hb = std::max(14, r.y + H - top - 26 - etich);
        barra(t, {xc + 4, top + 14, 8, hb}, d.compenso_prua, sq);
        barra(t, {xc + 22, top + 14, 8, hb}, d.compenso_poppa, sq);
        etichetta(t, xc + 8, top + 16 + hb, "FWD", f.piccolo, false);
        etichetta(t, xc + 26, top + 16 + hb, "AFT", f.piccolo, false);
        lampada_rettangolare(t, {xc + 40, top + 14, 12, 8}, ColoreLampada::verde, d.pilota);
        etichetta_sx(t, xc + 55, top + 15, "AUTO", f.piccolo, false);
        area(aree, Pulto::governo, {xc + 40, top + 12, 40, 12}, "pilota_automatico");
        sette_segmenti(t, xc + 44, top + 30, formato("%2.0f", static_cast<double>(d.punti_rotta)), ColoreLampada::ambra, f.cifre);
        etichetta_sx(t, xc + 60, top + 32, "WPT", f.piccolo, false);
        // il radar di navigazione BPS-15 acceso: emette, l'ESM nemico lo sente
        lampada_rettangolare(t, {xc + 40, top + 46, 12, 8}, ColoreLampada::rossa, d.radar);
        etichetta_sx(t, xc + 55, top + 47, "RADAR", f.piccolo, false);
        const int lato = std::min(40, r.y + H - top - etich - 6), xs = c.x + c.w - lato - 8;
        pulsante_coperto(t, {xs, top, lato, lato}, d.coperchio_soffiaggio, d.soffiaggio);
        etichetta(t, xs + lato / 2, top + lato + 2, "EMER BLOW", f.piccolo, false);
        area(aree, Pulto::bcp, {xs, top, lato, lato}, "emersione_emergenza");
    } else {
        targhetta(t, {c.x + 6, c.y + 4, 0, 0}, "POKS", f.targa);
        // modi di Boksit per rotta e quota (programma, automatico, manuale) e l'assetto Tan
        const int bh = std::max(9, std::min(14, (r.y + H - top - 6) / 4)), bw = 44;
        // la rotta: RUCHN e AVTO girano i modi (boksit_rotta), PROGR carica il programma dai punti di rotta (boksit_tappa) e
        // mostra la tappa in corso («P 3/7»); la quota: tutti e tre girano i modi (boksit_quota)
        const char* MODI[3] = {"RUCHN", "AVTO", "PROGR"};
        for (int riga = 0; riga < 2; ++riga) {
            const DatiGoverno::Modo m = riga == 0 ? d.modo_rotta : d.modo_quota;
            const int y = top + riga * (bh + 2);
            etichetta_sx(t, c.x + 6, y + (bh - f.piccolo.righe()) / 2, riga == 0 ? "KURS" : "GLUB", f.piccolo, true);
            for (int k = 0; k < 3; ++k) {
                const Rettangolo p{c.x + 38 + k * (bw + 2), y, bw, bh};
                std::string scritta = MODI[k];
                if (riga == 0 && k == 2 && d.tappe > 0) scritta = "P " + std::to_string(d.tappa) + "/" + std::to_string(d.tappe);
                pulsante(t, p, scritta, ColoreLampada::ambra, static_cast<int>(m) == k, static_cast<int>(m) == k, f.piccolo);
                area(aree, Pulto::boksit, p, riga == 1 ? "boksit_quota" : k == 2 ? "boksit_tappa" : "boksit_rotta");
            }
        }
        // ПОКС: Tan (assetto automatico), soffia e sfoga le casse, compensa imbarcando (+) o espellendo (-)
        const int y2 = top + 2 * (bh + 2) + 2;
        const Rettangolo tan{c.x + 38, y2, bw, bh};
        pulsante(t, tan, "TAN", ColoreLampada::verde, d.tan, false, f.piccolo);
        // la stazione radar (RLS) accesa: emette, l'ESM nemico la sente
        lampada_tonda(t, c.x + 10, y2 + bh / 2, 3, ColoreLampada::rossa, d.radar);
        etichetta_sx(t, c.x + 16, y2 + (bh - f.piccolo.righe()) / 2, "RLS", f.piccolo, true);
        area(aree, Pulto::tan, tan, "tan");
        const char* POKS[2] = {"PRODUV", "VENTIL"};
        const char* CMD[2] = {"emersione_emergenza", "sfoga"};   // soffiare le casse e' l'emersione d'emergenza
        const bool acc[2] = {d.soffia, d.sfoga};
        for (int k = 0; k < 2; ++k) {
            const Rettangolo p{c.x + 38 + (k + 1) * (bw + 2), y2, bw, bh};
            pulsante(t, p, POKS[k], ColoreLampada::verde, acc[k], acc[k], f.piccolo);
            area(aree, Pulto::boksit, p, CMD[k]);
        }
        const int y3 = y2 + bh + 2;
        const bool piu = d.compensa && d.compensa_verso >= 0, meno = d.compensa && d.compensa_verso < 0;
        const Rettangolo up{c.x + 38, y3, bw, bh}, um{c.x + 38 + bw + 2, y3, bw, bh};
        pulsante(t, up, "URAV+", ColoreLampada::verde, piu, piu, f.piccolo);
        area(aree, Pulto::boksit, up, "compensa_piu");
        pulsante(t, um, "URAV-", ColoreLampada::verde, meno, meno, f.piccolo);
        area(aree, Pulto::boksit, um, "compensa_meno");
        const Rettangolo fin{c.x + 38 + 2 * (bw + 2), y3, bw, bh};
        finestrella(t, fin, "AVAR", d.guasto_casse, f.piccolo);
        const int lato = std::min(40, r.y + H - top - etich - 6), xs = c.x + c.w - lato - 8;
        pulsante_coperto(t, {xs, top, lato, lato}, d.coperchio_soffiaggio, d.soffiaggio);
        etichetta(t, xs + lato / 2, top + lato + 2, "PRODUV", f.piccolo, true);   // d'emergenza: lo dice il coperchio
        area(aree, Pulto::boksit, {xs, top, lato, lato}, "emersione_emergenza");
    }
    t.imposta_ritaglio(prima);
}

// --- Armi ---

void pannello_armi(Tela& t, Rettangolo r, const TemaStazione& tema, const FontConsolle& f, const DatiArmi& d,
                   std::vector<AreaPulto>* aree) {
    const bool alfa = e_alfa(tema);
    const Rettangolo prima = t.ritaglio();
    t.imposta_ritaglio(r);
    const int H = r.h, top = r.y + 4 + f.targa.righe() + 3;
    const int n = static_cast<int>(d.tubi.size());

    // A. lo schermo del calcolatore: Mk 81 (688) o Akkord coi suoi tre bersagli (Alfa)
    const Rettangolo a{r.x, r.y, 236, H};
    pannello(t, a, tema.battello, 7);
    targhetta(t, {a.x + 6, a.y + 4, 0, 0}, alfa ? "AKKORD" : "MK 81 FIRE CONTROL", f.targa);
    const int hs = alfa ? r.y + H - top - 18 : r.y + H - top - 4;
    const Rettangolo v = crt_rettangolare(t, {a.x + 6, top - 2, alfa ? 150 : 224, hs});
    testo_fosforo_nel_vetro(t, v, d.schermo, alfa ? f.piccolo : f.schermo);   // a capo nel vetro, il resto tagliato
    area(aree, alfa ? Pulto::akkord : Pulto::tiro_mk81, {a.x + 6, top - 2, alfa ? 150 : 224, hs});
    if (alfa) {   // i tre bersagli di Akkord, l'avaria che blocca tutto
        // gli slot: la lampada (ambra lo scelto), la lettera del contatto e la distanza se c'e'; sotto DESIGNA, che mette
        // il contatto selezionato nel primo slot libero
        for (int k = 0; k < 3; ++k) {
            const int y = top + k * 14;
            lampada_tonda(t, a.x + 166, y + 5, 4, k == d.bersaglio_scelto ? ColoreLampada::ambra : ColoreLampada::verde, k < d.bersagli_akkord);
            const bool pieno = k < static_cast<int>(d.slot_akkord.size()) && !d.slot_akkord[static_cast<std::size_t>(k)].empty();
            etichetta_sx(t, a.x + 176, y + 1, pieno ? std::to_string(k + 1) + " " + d.slot_akkord[static_cast<std::size_t>(k)] : "CEL " + std::to_string(k + 1),
                         f.piccolo, true);
            area(aree, Pulto::akkord, {a.x + 158, y, 74, 12}, "akkord_" + std::to_string(k + 1));
        }
        const Rettangolo designa{a.x + 160, top + 3 * 14 + 1, 70, 12};
        pulsante(t, designa, "TSU", ColoreLampada::ambra, false, false, f.piccolo);   // ЦУ, tseleukazanie: la designazione
        area(aree, Pulto::akkord, designa, "akkord_designa");
        finestrella(t, {a.x + 6, r.y + H - 16, 224, 13}, "AKKORD AVARIJA", d.akkord_guasto, f.piccolo);
    }

    // B. i tubi: WCP del 688 (lampade per tubo, FIRE coperto) o Sargan dell'Alfa (pulsanti per tubo)
    const Rettangolo b{r.x + 240, r.y, r.w - 240, H};
    pannello(t, b, tema.battello, 7);
    targhetta(t, {b.x + 6, b.y + 4, 0, 0}, alfa ? "SARGAN" : "WEAPON CONTROL PANEL", f.targa);
    if (n == 0) { t.imposta_ritaglio(prima); return; }
    if (!alfa) {
        const int lato = std::min(44, r.y + H - top - f.piccolo.righe() - 6), xf = b.x + b.w - lato - 8;
        const int x0 = b.x + 34, larg = (xf - 8 - x0) / n;
        const char* SPIE[4] = {"RDY", "FLD", "DOOR", "WIRE"};
        const int sh = std::max(7, std::min(11, (r.y + H - top - f.piccolo.righe() - 8) / 4));
        for (int s = 0; s < 4; ++s) etichetta_sx(t, b.x + 6, top + f.piccolo.righe() + 3 + s * sh + (sh - f.piccolo.righe()) / 2, SPIE[s], f.piccolo, false);
        for (int i = 0; i < n; ++i) {
            const auto& tb = d.tubi[static_cast<std::size_t>(i)];
            const int x = x0 + i * larg;
            const Rettangolo col{x, top - 2, larg - 4, r.y + H - top - 2};
            if (i == d.selezionato) {   // il tubo scelto: la cornice chiara
                t.linea_orizzontale(col.x, col.x + col.w - 1, col.y, smalto(11));
                t.linea_orizzontale(col.x, col.x + col.w - 1, col.y + col.h - 1, smalto(11));
                t.linea_verticale(col.x, col.y, col.y + col.h - 1, smalto(11));
                t.linea_verticale(col.x + col.w - 1, col.y, col.y + col.h - 1, smalto(11));
            }
            etichetta_sx(t, x + 3, top, std::to_string(i + 1) + " " + (tb.arma.empty() ? "-" : tb.arma), f.piccolo, false);
            const bool spie[4] = {tb.pronto, tb.allagato, tb.porta, tb.filo};
            const ColoreLampada colore[4] = {ColoreLampada::verde, ColoreLampada::ambra, ColoreLampada::ambra, ColoreLampada::verde};
            for (int s = 0; s < 4; ++s)
                lampada_rettangolare(t, {x + 3, top + f.piccolo.righe() + 3 + s * sh, larg - 12, sh - 2}, colore[s], spie[s]);
            if (tb.ricarica >= 0.0) {   // ricarica: la barretta ambra sotto le spie
                const int w = static_cast<int>((larg - 12) * std::clamp(tb.ricarica, 0.0, 1.0));
                t.rettangolo_pieno(x + 3, col.y + col.h - 3, w, 2, lampada(ColoreLampada::ambra, true));
            }
            area(aree, Pulto::wcp, col, "tubo_" + std::to_string(i + 1));
        }
        pulsante_coperto(t, {xf, top, lato, lato}, d.fuoco_coperchio, false);
        etichetta(t, xf + lato / 2, top + lato + 2, "FIRE", f.piccolo, false);
        area(aree, Pulto::wcp, {xf, top, lato, lato}, "lancia");
    } else {
        // una fila per tubo; se i pulsanti verrebbero piu' bassi della scritta, due colonne di tubi
        const bool due = (r.y + H - top) / n < f.piccolo.righe() + 4 && n > 1;
        const int righe_col = due ? (n + 1) / 2 : n, larg_col = due ? (b.w - 4) / 2 : b.w - 4;
        const int rh = std::max(8, (r.y + H - top) / righe_col);
        const char* PULS[4] = {"GOT", "ZAL", "KRYSH", "PUSK"};
        const int etich = due ? 60 : 86;   // numero e arma prima dei pulsanti
        const int bw = std::min(54, (larg_col - etich - 2) / 4 - 2);
        for (int i = 0; i < n; ++i) {
            const auto& tb = d.tubi[static_cast<std::size_t>(i)];
            const int col = due && i >= righe_col ? 1 : 0;
            const int y = top + (i - col * righe_col) * rh, x = b.x + 2 + col * larg_col;
            etichetta_sx(t, x + 4, y + (rh - 1 - f.piccolo.righe()) / 2, std::to_string(i + 1), f.piccolo, true);
            etichetta_sx(t, x + 14, y + (rh - 1 - f.piccolo.righe()) / 2, tb.arma.empty() ? "-" : tb.arma, f.piccolo, true);
            const bool sel = i == d.selezionato;
            const bool acc[4] = {tb.pronto, tb.allagato, tb.porta, sel && tb.pronto && tb.porta && !d.akkord_guasto};
            const int xp = x + etich;
            for (int k = 0; k < 4; ++k) {
                const Rettangolo p{xp + k * (bw + 2), y, bw, rh - 1};
                pulsante(t, p, PULS[k], k == 3 ? ColoreLampada::rossa : ColoreLampada::verde, acc[k], false, f.piccolo);
            }
            if (tb.ricarica >= 0.0) {   // ricarica automatica UBZ: la barretta sotto i pulsanti
                const int w = static_cast<int>((4 * (bw + 2) - 2) * std::clamp(tb.ricarica, 0.0, 1.0));
                t.rettangolo_pieno(xp, y + rh - 2, w, 1, lampada(ColoreLampada::ambra, true));
            }
            if (sel) t.rettangolo_pieno(x, y, 2, rh - 1, lampada(ColoreLampada::ambra, true));
            area(aree, Pulto::sargan, {x, y, etich + 3 * (bw + 2), rh - 1}, "tubo_" + std::to_string(i + 1));
            if (sel) area(aree, Pulto::sargan, {xp + 3 * (bw + 2), y, bw, rh - 1}, d.akkord_guasto ? "" : "lancia");
        }
    }
    t.imposta_ritaglio(prima);
}

// --- Stato dell'Alfa: il pulto di Ritm ---

void pannello_stato_alfa(Tela& t, Rettangolo r, const TemaStazione& tema, const FontConsolle& f, const DatiStato& d,
                         std::vector<AreaPulto>* aree) {
    const Rettangolo prima = t.ritaglio();
    t.imposta_ritaglio(r);
    const int H = r.h, top = r.y + 4 + f.targa.righe() + 3;
    // A. sinottici a lampade (reattore Gamma, rete Tembr, impianti Takt-T)
    const Rettangolo a{r.x, r.y, 300, H};
    pannello(t, a, tema.battello, 7);
    targhetta(t, {a.x + 6, a.y + 4, 0, 0}, "RITM", f.targa);
    const int n = std::max<int>(1, static_cast<int>(d.sinottici.size()));
    const int rh = std::max(9, (r.y + H - top - 2) / n);
    for (std::size_t i = 0; i < d.sinottici.size(); ++i) {
        const auto& s = d.sinottici[i];
        const int y = top + static_cast<int>(i) * rh;
        etichetta_sx(t, a.x + 6, y + (rh - f.piccolo.righe()) / 2, s.nome, f.piccolo, true);
        for (std::size_t k = 0; k < s.lampade.size(); ++k)
            lampada_tonda(t, a.x + 70 + static_cast<int>(k) * 14, y + rh / 2, std::max(2, std::min(4, rh / 2 - 2)), s.lampade[k],
                          k < s.accese.size() && s.accese[k]);
        area(aree, Pulto::ritm, {a.x + 2, y, a.w - 4, rh});
    }
    // B. quadranti a fondo bianco: potenza del reattore e temperatura
    const Rettangolo b{r.x + 304, r.y, 160, H};
    pannello(t, b, tema.battello, 7);
    targhetta(t, {b.x + 6, b.y + 4, 0, 0}, "GAMMA", f.targa);
    const int rq = std::max(10, std::min(30, (r.y + H - top - f.piccolo.righe() - 4) / 2));
    const int cyq = top + rq + 1;
    quadrante(t, b.x + 8 + rq, cyq, rq, 100.0 * d.potenza_reattore, {0, 100, -135, 135, 10, rq >= 22 ? 2 : 0, 1.0},
              StileQuadrante::bianco_alfa, &f.piccolo);
    etichetta(t, b.x + 8 + rq, cyq + rq + 2, "MOSHCH %", f.piccolo, true);
    quadrante(t, b.x + b.w - 8 - rq, cyq, rq, d.temperatura, {0, 400, -135, 135, 8, rq >= 22 ? 2 : 0, 1.0},
              StileQuadrante::bianco_alfa, &f.piccolo);
    etichetta(t, b.x + b.w - 8 - rq, cyq + rq + 2, "TEMP C", f.piccolo, true);
    area(aree, Pulto::ritm, b);
    // C. finestrelle d'allarme: le accese in alto, le altre spente
    const Rettangolo c{r.x + 468, r.y, r.w - 468, H};
    pannello(t, c, tema.battello, 7);
    targhetta(t, {c.x + 6, c.y + 4, 0, 0}, "AVARIJA", f.targa);
    const int fh = std::max(10, std::min(14, (r.y + H - top - 2) / 4));
    for (int k = 0; k < 4; ++k) {
        const bool accesa = k < static_cast<int>(d.allarmi.size());
        finestrella(t, {c.x + 6, top + k * (fh + 2), c.w - 12, fh}, accesa ? d.allarmi[static_cast<std::size_t>(k)] : "", accesa, f.piccolo);
    }
    area(aree, Pulto::ritm, c);
    t.imposta_ritaglio(prima);
}

// --- Radio ed ESM ---

void pannello_radio_esm(Tela& t, Rettangolo r, const TemaStazione& tema, const FontConsolle& f, const DatiRadioEsm& d,
                        std::vector<AreaPulto>* aree) {
    const bool alfa = e_alfa(tema);
    const Rettangolo prima = t.ritaglio();
    t.imposta_ritaglio(r);
    const int H = r.h, top = r.y + 4 + f.targa.righe() + 3;
    // A. la telescrivente: carta chiara, le ultime righe in basso (UGC-136 del 688, telescrivente sovietica grigia)
    const bool con_pulsanti = !d.pulsanti.empty();
    const int wa = con_pulsanti ? 230 : 330, wb = con_pulsanti ? 222 : r.w - 334;
    const Rettangolo a{r.x, r.y, wa, H};
    pannello(t, a, tema.battello, 7);
    targhetta(t, {a.x + 6, a.y + 4, 0, 0}, alfa ? "MOLNIJA" : "RADIO ROOM  UGC-136", f.targa);
    const Rettangolo carta{a.x + 8, top, a.w - 16, r.y + H - top - 5};
    t.rettangolo_pieno(carta.x, carta.y, carta.w, carta.h, smalto(alfa ? 9 : 11));   // la carta prende la luce della sala
    t.linea_orizzontale(carta.x, carta.x + carta.w - 1, carta.y + carta.h - 1, smalto(4));
    const Font& ft = alfa ? f.piccolo : f.schermo;
    const int righe = std::max(1, carta.h / (ft.righe() + 1));
    const int da = std::max(0, static_cast<int>(d.righe_telescrivente.size()) - righe);
    for (int i = da; i < static_cast<int>(d.righe_telescrivente.size()); ++i)
        ft.scrivi(t, carta.x + 3, carta.y + 1 + (i - da) * (ft.righe() + 1), d.righe_telescrivente[static_cast<std::size_t>(i)],
                  static_cast<std::uint8_t>(colori::NERO), 1, spazio(ft), false);
    area(aree, alfa ? Pulto::molnija : Pulto::radio, a);
    // B. ESM: la panoramica sul CRT (WLR-8 a due tracce; sull'Alfa la sezione Bukhta col CRT tondo), le intercettazioni
    const Rettangolo b{r.x + wa + 4, r.y, wb, H};
    pannello(t, b, tema.battello, 7);
    targhetta(t, {b.x + 6, b.y + 4, 0, 0}, alfa ? "BUKHTA" : "WLR-8 ESM", f.targa);
    Rettangolo v;
    if (alfa) {
        // il CRT tondo sotto la cappa (che sporge 9 pixel): sotto la targhetta
        const int rr = std::max(10, std::min(36, (r.y + H - top - 22) / 2));
        crt_tondo(t, b.x + 16 + rr, top + 8 + rr, rr);
        v = {b.x + 16, top + 8 + rr / 2, 2 * rr, rr};
    } else {
        v = crt_rettangolare(t, {b.x + 6, top - 2, con_pulsanti ? 104 : 150, r.y + H - top - 2});
    }
    if (!d.panoramica.empty() && v.w > 4 && v.h > 4) {   // la traccia: un punto per colonna, dal basso
        const int nb = static_cast<int>(d.panoramica.size());
        int yp = -1;
        for (int x = 0; x < v.w; ++x) {
            const float l = d.panoramica[static_cast<std::size_t>(x * nb / v.w)];
            const int y = v.y + v.h - 2 - static_cast<int>(std::clamp(l, 0.0f, 1.0f) * static_cast<float>(v.h - 4));
            if (yp >= 0) t.linea_verticale(v.x + x, std::min(y, yp), std::max(y, yp), fosforo(5));
            t.pixel(v.x + x, y, fosforo(7));
            yp = y;
        }
    }
    const int xl = alfa ? b.x + 110 : b.x + (con_pulsanti ? 116 : 162);
    lampada_rettangolare(t, {xl, top, 34, 9}, ColoreLampada::rossa, d.minaccia);
    etichetta_sx(t, xl + 37, top + 1, alfa ? "UGROZA" : "THREAT", f.piccolo, alfa);
    if (alfa) {   // le intercettazioni a stampino sul pannello crema
        const Font& fl = f.piccolo;
        for (std::size_t i = 0; i < d.intercettazioni.size(); ++i) {
            const int y = top + 13 + static_cast<int>(i) * (fl.righe() + 1);
            if (y + fl.righe() > r.y + H - 3) break;
            etichetta_sx(t, xl, y, d.intercettazioni[i], fl, alfa);
        }
    } else {   // 688: sulla finestrella del WLR-8 (fosforo: si legge anche con la luce rossa o blu della sala)
        const Rettangolo q = crt_rettangolare(t, {xl - 2, top + 11, b.x + b.w - 6 - (xl - 2), r.y + H - top - 14});
        const Font& fl = f.schermo;
        for (std::size_t i = 0; i < d.intercettazioni.size(); ++i) {
            const int y = q.y + 2 + static_cast<int>(i) * (fl.righe() + 1);
            if (y + fl.righe() > q.y + q.h) break;
            testo_fosforo(t, q.x + 2, y, d.intercettazioni[i], fl);
        }
    }
    area(aree, alfa ? Pulto::molnija : Pulto::esm, b);
    if (con_pulsanti) {   // C. i pulsanti della sala radio, 3 colonne
        const Rettangolo c{b.x + b.w + 4, r.y, r.x + r.w - (b.x + b.w + 4), H};
        pannello(t, c, tema.battello, 7);
        targhetta(t, {c.x + 6, c.y + 4, 0, 0}, alfa ? "PULT" : "COMMS", f.targa);
        const int n = std::min<int>(9, static_cast<int>(d.pulsanti.size())), file = (n + 2) / 3;
        const int bw = (c.w - 12) / 3 - 2, bh = std::max(8, (r.y + H - top - 2) / std::max(1, file) - 2);
        for (int i = 0; i < n; ++i) {
            const auto& p = d.pulsanti[static_cast<std::size_t>(i)];
            const Rettangolo q{c.x + 6 + (i % 3) * (bw + 2), top + (i / 3) * (bh + 2), bw, bh};
            pulsante(t, q, p.etichetta, alfa ? ColoreLampada::verde : ColoreLampada::ambra, p.acceso, p.acceso, f.piccolo);
            area(aree, alfa ? Pulto::molnija : Pulto::radio, q, p.comando);
        }
    }
    t.imposta_ritaglio(prima);
}

// --- Okean ---

void schermo_okean(Tela& t, int cx, int cy, int r, const std::vector<float>& livello_per_rilevamento, double fascio,
                   const FontConsolle& f) {
    crt_tondo(t, cx, cy, r);
    // il ventaglio (STIMA: la presentazione di Okean non e' nota, dossier 19 §6.6): rilevamenti relativi -120..+120 dal
    // centro dello schermo, la prua in alto; ogni raggio lungo quanto il livello, piu' luminoso piu' forte
    const int n = static_cast<int>(livello_per_rilevamento.size());
    const double xc = cx + 0.5, yc = cy + 0.5;
    for (int g = -120; g <= 120; ++g) {
        const float l = n ? livello_per_rilevamento[static_cast<std::size_t>(((g % 360) + 360) % 360 * n / 360)] : 0.0f;
        const double a = g * PI / 180.0, len = (0.15 + 0.8 * std::clamp(l, 0.0f, 1.0f)) * r;
        const int tono = 1 + static_cast<int>(std::clamp(l, 0.0f, 1.0f) * 5);
        for (double k = 2; k < len; k += 1.0)
            t.pixel(static_cast<int>(xc + k * std::sin(a)), static_cast<int>(yc - k * std::cos(a)), fosforo(tono));
    }
    // il bordo del settore e il fascio puntato
    for (const int g : {-120, 120}) {
        const double a = g * PI / 180.0;
        for (double k = 2; k < r - 1; k += 2) t.pixel(static_cast<int>(xc + k * std::sin(a)), static_cast<int>(yc - k * std::cos(a)), fosforo(3));
    }
    const double a = fascio * PI / 180.0;
    for (double k = 2; k < r - 1; k += 1) t.pixel(static_cast<int>(xc + k * std::sin(a)), static_cast<int>(yc - k * std::cos(a)), fosforo(7));
    testo_fosforo(t, cx - f.piccolo.larghezza("0", 1, spazio(f.piccolo), false) / 2, cy - r + 3, "0", f.piccolo);
}

// --- Centrale ---

namespace {

// poligono convesso pieno (scansione per righe)
void poligono(Tela& t, const std::vector<std::pair<double, double>>& p, std::uint8_t c) {
    if (p.size() < 3) return;
    double y0 = p[0].second, y1 = p[0].second;
    for (const auto& q : p) { y0 = std::min(y0, q.second); y1 = std::max(y1, q.second); }
    for (int y = static_cast<int>(std::ceil(y0)); y <= static_cast<int>(std::floor(y1)); ++y) {
        double xa = 1e9, xb = -1e9;
        for (std::size_t i = 0; i < p.size(); ++i) {
            const auto& a = p[i];
            const auto& b = p[(i + 1) % p.size()];
            if ((y < std::min(a.second, b.second)) || (y > std::max(a.second, b.second)) || a.second == b.second) continue;
            const double x = a.first + (b.first - a.first) * (y - a.second) / (b.second - a.second);
            xa = std::min(xa, x);
            xb = std::max(xb, x);
        }
        if (xa <= xb) t.linea_orizzontale(static_cast<int>(std::lround(xa)), static_cast<int>(std::lround(xb)), y, c);
    }
}

// un mobile di consolle visto di fronte con il piano inclinato sopra: il fronte in r, il piano alto `alto` pixel
// che rientra di `rientro` (prospettiva semplice), smalto del tono dato
void mobile(Tela& t, Rettangolo r, int alto, int rientro, int tono) {
    poligono(t, {{r.x + rientro, r.y - alto}, {r.x + r.w - rientro, r.y - alto}, {r.x + r.w - 1.0, r.y}, {r.x, r.y}}, smalto(tono + 2));
    t.rettangolo_pieno(r.x, r.y, r.w, r.h, smalto(tono));
    t.linea_orizzontale(r.x, r.x + r.w - 1, r.y, smalto(tono + 3));
    t.linea_verticale(r.x + r.w - 1, r.y, r.y + r.h - 1, smalto(tono - 2));
}

// cilindro verticale (periscopio, colonna): ombreggiato da sinistra a destra
void cilindro(Tela& t, int x, int y0, int y1, int w, int chiaro) {
    for (int k = 0; k < w; ++k) {
        const double u = (k + 0.5) / w;   // 0 sinistra, 1 destra
        const int tono = chiaro - static_cast<int>(std::lround(5.0 * std::fabs(u - 0.35) * 2.0));
        t.linea_verticale(x + k, y0, y1, smalto(tono));
    }
}

// una figurina seduta di spalle (gli uomini di guardia): testa, spalle, schienale della poltrona
void figurina(Tela& t, int cx, int ybase, int scala, std::uint8_t divisa, std::uint8_t schienale, std::uint8_t cuscino) {
    const int s = std::max(1, scala);
    t.rettangolo_pieno(cx - 5 * s, ybase - 14 * s, 10 * s, 14 * s, schienale);    // lo schienale
    t.rettangolo_pieno(cx - 4 * s, ybase - 13 * s, 8 * s, 4 * s, cuscino);
    t.rettangolo_pieno(cx - 4 * s, ybase - 20 * s, 8 * s, 8 * s, divisa);         // le spalle sopra lo schienale
    for (int y = -3 * s; y <= 3 * s; ++y)                                          // la testa
        for (int x = -3 * s; x <= 3 * s; ++x)
            if (x * x + y * y <= 9 * s * s) t.pixel(cx + x, ybase - 24 * s + y, smalto(2));
}

// lampadine del pannello che lampeggiano piano col tempo (deterministiche)
void lucine(Tela& t, Rettangolo r, int n, double tempo_s, int seme) {
    for (int i = 0; i < n; ++i) {
        const int x = r.x + 2 + (i * 7 + seme * 3) % std::max(1, r.w - 4), y = r.y + 2 + (i * 5 + seme) % std::max(1, r.h - 4);
        const auto c = static_cast<ColoreLampada>((i + seme) % 3);
        const bool accesa = std::fmod(tempo_s * (0.2 + 0.07 * ((i + seme) % 5)) + i * 0.37, 1.0) < 0.75;
        t.pixel(x, y, lampada(c, accesa));
        t.pixel(x + 1, y, lampada(c, accesa));
    }
}

void schermino(Tela& t, Rettangolo r, double tempo_s, int seme) {   // un piccolo CRT acceso con righe di testo finto
    t.rettangolo_pieno(r.x - 1, r.y - 1, r.w + 2, r.h + 2, static_cast<std::uint8_t>(colori_tema::ACCENTO));
    t.rettangolo_pieno(r.x, r.y, r.w, r.h, fosforo(0));
    for (int y = r.y + 2; y < r.y + r.h - 2; y += 3) {
        const int w = 3 + (y * 13 + seme * 7 + static_cast<int>(tempo_s / 3)) % std::max(1, r.w - 6);
        t.linea_orizzontale(r.x + 2, r.x + 2 + w, y, fosforo(4));
    }
}

}  // namespace

void vista_centrale(Tela& t, const TemaStazione& tema, const FontConsolle& f, double tempo_s, std::vector<AreaPulto>* aree) {
    const bool alfa = e_alfa(tema);
    const Rettangolo prima = t.ritaglio();
    const int W = LARGHEZZA_TELA, H = ALTEZZA_VISTA;
    t.imposta_ritaglio({0, 0, W, H});
    const auto targa = [&](int x, int y, const char* s) { targhetta(t, {x, y, 0, 0}, s, f.piccolo); };
    if (!alfa) {
        // 688: la sala vista dall'ufficiale di guardia. Cielo con le tubolature, paratia di fondo, pagliolato
        t.rettangolo_pieno(0, 0, W, H, smalto(2));
        t.rettangolo_pieno(0, 0, W, 34, smalto(1));
        for (int k = 0; k < 4; ++k) {
            t.linea_orizzontale(0, W - 1, 8 + k * 7, smalto(4));
            t.linea_orizzontale(0, W - 1, 9 + k * 7, smalto(0));
        }
        poligono(t, {{0, 300}, {W - 1.0, 300}, {W - 1.0, H - 1.0}, {0, H - 1.0}}, smalto(1));
        for (int x = -200; x < W + 200; x += 60)   // le giunture del pagliolato verso il fondo
            poligono(t, {{x + 0.0, 300}, {x + 1.0, 300}, {320 + (x - 320) * 1.6 + 1, H - 1.0}, {320 + (x - 320) * 1.6, H - 1.0}}, smalto(0));
        // il quadro degli allagamenti sulla paratia (Flood Control Gauge Board, dossier 19 §6.7): la stazione Stato
        pannello(t, {30, 44, 156, 32}, BattelloTema::usa688, 5);
        targa(34, 47, "FLOOD CONTROL");
        for (int k = 0; k < 8; ++k) lampada_rettangolare(t, {36 + k * 18, 62, 14, 8}, k == 7 ? ColoreLampada::rossa : ColoreLampada::verde, k != 7);
        area(aree, Pulto::bcp, {30, 44, 156, 32}, "stazione_stato");
        // BCP curvo a sinistra in fondo, con l'albero di Natale e le scatole d'allarme
        mobile(t, {24, 92, 170, 120}, 10, 6, 6);
        targa(30, 96, "BALLAST CONTROL");
        std::vector<bool> ap(24, false);
        albero_di_natale(t, {32, 112, 90, 40}, ap, 8);
        lucine(t, {128, 112, 60, 40}, 14, tempo_s, 1);
        for (int k = 0; k < 3; ++k) lampada_rettangolare(t, {34 + k * 30, 160, 24, 10}, static_cast<ColoreLampada>(k), false);
        area(aree, Pulto::bcp, {24, 82, 170, 130}, "stazione_manovra");
        // governo: il pannello con i quadranti e le due colonne, i due sedili davanti
        mobile(t, {10, 226, 210, 74}, 14, 10, 6);
        targa(16, 230, "SHIP CONTROL");
        for (int k = 0; k < 4; ++k) quadrante(t, 40 + k * 46, 262, 15, 40.0 * k + std::fmod(tempo_s, 30.0), {}, StileQuadrante::scuro_688);
        cloche(t, {40, 286, 60, 40}, 0.0, 0.0);
        cloche(t, {130, 286, 60, 40}, 0.1, -0.1);
        figurina(t, 70, 352, 2, smalto(4), smalto(1), smalto(3));
        figurina(t, 160, 352, 2, smalto(4), smalto(1), smalto(3));
        area(aree, Pulto::governo, {10, 212, 210, 148}, "stazione_manovra");
        // la pedana coi due periscopi, dal cielo alla pedana
        poligono(t, {{226, 268}, {414, 268}, {440, 300}, {200, 300}}, smalto(5));
        t.linea_orizzontale(200, 440, 300, smalto(8));
        for (int x = 210; x <= 430; x += 44) t.linea_verticale(x, 254, 268, smalto(8));   // la ringhiera
        t.linea_orizzontale(206, 434, 254, smalto(8));
        cilindro(t, 258, 30, 268, 28, 8);   // Type 2 d'attacco (sottile in testa, qui il tubo)
        cilindro(t, 352, 30, 268, 34, 8);   // Type 18 di ricerca
        t.rettangolo_pieno(250, 200, 44, 16, smalto(3));   // gli oculari e le maniglie
        t.rettangolo_pieno(342, 196, 54, 20, smalto(3));
        t.rettangolo_pieno(240, 204, 10, 4, smalto(1));
        t.rettangolo_pieno(294, 204, 10, 4, smalto(1));
        t.rettangolo_pieno(330, 202, 12, 5, smalto(1));
        t.rettangolo_pieno(396, 202, 12, 5, smalto(1));
        etichetta(t, 272, 234, "TYPE 2", f.piccolo, false);
        etichetta(t, 369, 234, "TYPE 18", f.piccolo, false);
        area(aree, Pulto::periscopio_attacco, {240, 30, 64, 238}, "stazione_periscopio");
        area(aree, Pulto::periscopio_ricerca, {330, 30, 78, 238}, "stazione_periscopio");
        // fila delle consolle di tiro Mk 81 a destra, il WCP in fondo
        for (int k = 0; k < 3; ++k) {
            const int x = 446 + k * 62;
            mobile(t, {x, 120 + k * 10, 56, 170 - k * 10}, 10, 4, 6);
            schermino(t, {x + 6, 130 + k * 10, 44, 34}, tempo_s, k);
            lucine(t, {x + 4, 172 + k * 10, 48, 18}, 6, tempo_s, k + 5);
            for (int m = 0; m < 3; ++m) manopola(t, x + 12 + m * 16, 206 + k * 10, 5, 30.0 * m);
        }
        targa(452, 104, "MK 81 FIRE CONTROL");
        area(aree, Pulto::tiro_mk81, {446, 96, 186, 194}, "stazione_armi");
        mobile(t, {446, 300, 186, 40}, 6, 4, 5);
        targa(452, 304, "WCP");
        for (int k = 0; k < 4; ++k) lampada_rettangolare(t, {490 + k * 30, 306, 24, 9}, ColoreLampada::verde, k == 0);
        pulsante_coperto(t, {612, 304, 18, 18}, false, false);
        area(aree, Pulto::wcp, {446, 294, 186, 46}, "stazione_armi");
        // tavolo di carteggio in primo piano, la carta a matita
        poligono(t, {{238, 318}, {402, 318}, {430, 359}, {210, 359}}, smalto(4));
        poligono(t, {{248, 322}, {392, 322}, {414, 356}, {226, 356}}, smalto(11));
        for (int k = 0; k < 5; ++k) t.linea_orizzontale(240 + k * 3, 400 - k * 3, 328 + k * 6, smalto(8));
        area(aree, Pulto::carteggio, {210, 310, 220, 50}, "stazione_navigazione");
        // ESM e porte: sala radio a sinistra, sala sonar (luce blu) a destra
        mobile(t, {196, 156, 40, 70}, 8, 3, 6);
        schermino(t, {200, 162, 32, 22}, tempo_s, 9);
        targa(198, 146, "ESM");
        area(aree, Pulto::esm, {196, 144, 40, 82}, "stazione_radio");
        t.rettangolo_pieno(0, 40, 18, 180, smalto(0));
        etichetta(t, 9, 44, "R", f.piccolo, false);
        area(aree, Pulto::radio, {0, 40, 18, 180}, "stazione_radio");
        t.rettangolo_pieno(W - 14, 40, 14, 60, static_cast<std::uint8_t>(colori_tema::ACCENTO + 1));
        etichetta(t, W - 7, 44, "S", f.piccolo, false);
        area(aree, Pulto::sonar, {W - 14, 40, 14, 60}, "stazione_sonar");
    } else {
        // Alfa: il ferro di cavallo visto dal pulto del comandante. Pavimento blu-grigio, cielo crema coi cordoni
        t.rettangolo_pieno(0, 0, W, H, smalto(9));
        poligono(t, {{0, 250}, {W - 1.0, 250}, {W - 1.0, H - 1.0}, {0, H - 1.0}}, smalto(1));
        poligono(t, {{0, 0}, {W - 1.0, 0}, {W - 1.0, 30}, {0, 30}}, smalto(10));
        for (int k = 0; k < 5; ++k) {   // i cordoni del telefono a spirale dal cielo
            const int x = 90 + k * 115;
            for (int y = 30; y < 70 + (k % 2) * 20; y += 2) t.pixel(x + ((y / 2) % 3) - 1, y, static_cast<std::uint8_t>(colori_tema::ACCENTO));
        }
        // in fondo al centro: il periscopio Signal e lo schermo della TV-1 del comandante
        cilindro(t, 300, 30, 250, 40, 10);
        t.rettangolo_pieno(292, 170, 56, 22, smalto(7));
        etichetta(t, 320, 196, "SIGNAL", f.piccolo, true);
        area(aree, Pulto::signal, {290, 30, 60, 220}, "stazione_periscopio");
        const Rettangolo tv = crt_rettangolare(t, {360, 150, 70, 54});
        schermino(t, {tv.x + 2, tv.y + 2, tv.w - 4, tv.h - 4}, tempo_s, 3);
        etichetta(t, 395, 208, "TV-1", f.piccolo, true);
        area(aree, Pulto::tv1, {360, 150, 70, 54}, "stazione_periscopio");
        // braccio sinistro: Molnija in fondo, Ritm, Boksit davanti
        const struct { Rettangolo r; Pulto p; const char* nome; const char* comando; } sx[3] = {
            {{186, 150, 96, 100}, Pulto::molnija, "MOLNIJA", "stazione_radio"},
            {{96, 160, 104, 120}, Pulto::ritm, "RITM", "stazione_stato"},
            {{0, 176, 112, 150}, Pulto::boksit, "BOKSIT", "stazione_manovra"}};
        for (int k = 0; k < 3; ++k) {
            const auto& m = sx[k];
            mobile(t, m.r, 12 - k * 2, 6, 7 + k);
            targhetta(t, {m.r.x + 6, m.r.y + 4, 0, 0}, m.nome, f.piccolo);
            lucine(t, {m.r.x + 6, m.r.y + 18, m.r.w - 12, 20}, 10 + 4 * k, tempo_s, k + 11);
            for (int q = 0; q < 2; ++q) quadrante(t, m.r.x + 22 + q * 34, m.r.y + 56 + 8 * k, 11 + 2 * k, 30.0 * q + 50.0 * k, {}, StileQuadrante::bianco_alfa);
            area(aree, m.p, {m.r.x, m.r.y - 12, m.r.w, m.r.h + 12}, m.comando);
        }
        // braccio destro: navigatore e Sozh in fondo, Okean, Sargan, Akkord davanti (i CRT tondi sotto le cappe)
        const struct { Rettangolo r; Pulto p; const char* nome; const char* comando; } dx[4] = {
            {{436, 150, 80, 100}, Pulto::navigatore, "SOZH", "stazione_navigazione"},
            {{506, 160, 70, 118}, Pulto::okean, "OKEAN", "stazione_sonar"},
            {{566, 168, 74, 132}, Pulto::sargan, "SARGAN", "stazione_armi"},
            {{520, 300, 120, 60}, Pulto::akkord, "AKKORD", "stazione_armi"}};
        for (int k = 0; k < 4; ++k) {
            const auto& m = dx[k];
            mobile(t, m.r, 10, 5, 7 + std::min(k, 2));
            targhetta(t, {m.r.x + 4, m.r.y + 4, 0, 0}, m.nome, f.piccolo);
            if (m.p == Pulto::okean || m.p == Pulto::akkord) {
                const int rr = m.p == Pulto::okean ? 18 : 14;
                crt_tondo(t, m.r.x + m.r.w / 2, m.r.y + 24 + rr, rr);
                schermo_okean(t, m.r.x + m.r.w / 2, m.r.y + 24 + rr, rr, std::vector<float>(36, 0.3f), std::fmod(tempo_s * 3, 240.0) - 120.0, f);
            } else {
                lucine(t, {m.r.x + 4, m.r.y + 18, m.r.w - 8, 30}, 12, tempo_s, k + 21);
            }
            area(aree, m.p, {m.r.x, m.r.y - 10, m.r.w, m.r.h + 10}, m.comando);
        }
        // gli uomini di guardia sulle poltroncine a guscio crema coi cuscini marrone
        const std::uint8_t divisa = smalto(1), guscio = smalto(11), cuscino = smalto(4);
        const int posti[5][3] = {{60, 352, 2}, {150, 300, 2}, {234, 262, 1}, {470, 262, 1}, {476, 352, 2}};
        for (const auto& p : posti) figurina(t, p[0], p[1], p[2], divisa, guscio, cuscino);
    }
    t.imposta_ritaglio(prima);
}

// --- Cornici ---

void cornice_periscopio(Tela& t, const TemaStazione& tema, const FontConsolle& f, bool tv, int periscopio) {
    const bool alfa = e_alfa(tema);
    const Rettangolo prima = t.ritaglio();
    const int W = LARGHEZZA_TELA, H = ALTEZZA_VISTA;
    t.imposta_ritaglio({0, 0, W, H});
    if (tv) {   // la TV-1 del comandante (o la TV del Type 18): la cornice grigia del monitor attorno a tutta la vista
        for (int k = 0; k < 10; ++k) {
            const std::uint8_t c = smalto(k < 2 ? 3 : k < 8 ? 6 : 4);
            t.linea_orizzontale(k, W - 1 - k, k, c);
            t.linea_orizzontale(k, W - 1 - k, H - 1 - k, c);
            t.linea_verticale(k, k, H - 1 - k, c);
            t.linea_verticale(W - 1 - k, k, H - 1 - k, c);
        }
        targhetta(t, {14, H - 24, 0, 0}, alfa ? "TV-1" : "TYPE 18  TV", f.piccolo);
        t.imposta_ritaglio(prima);
        return;
    }
    // le maniglie ai lati dell'oculare (fuori dal cerchio, che sta fra x 170 e 470): sul 688 a destra quella
    // dell'ingrandimento, sul Type 18 con i comandi (pulsanti dell'ESM e della TV); sull'Alfa le maniglie del Signal
    for (const int lato : {0, 1}) {
        const int x0 = lato ? W - 56 : 8;
        poligono(t, {{x0 + 0.0, 150}, {x0 + 48.0, 156}, {x0 + 48.0, 204}, {x0 + 0.0, 210}}, smalto(alfa ? 2 : 3));
        t.rettangolo_pieno(x0 + 6, 160, 36, 40, smalto(alfa ? 1 : 2));
        t.linea_orizzontale(x0 + 6, x0 + 41, 160, smalto(alfa ? 4 : 6));
        if (!alfa && periscopio == 2 && lato == 1) {   // i comandi della maniglia del Type 18
            lampada_rettangolare(t, {x0 + 10, 166, 12, 7}, ColoreLampada::verde, true);
            lampada_rettangolare(t, {x0 + 26, 166, 12, 7}, ColoreLampada::ambra, false);
        }
    }
    // in basso a sinistra: in alto c'e' la scala dei rilevamenti (righe 0-12 della vista)
    targhetta(t, {14, H - 24, 0, 0}, alfa ? "SIGNAL" : periscopio == 2 ? "TYPE 18 SEARCH" : "TYPE 2 ATTACK", f.piccolo);
    t.imposta_ritaglio(prima);
}

Rettangolo cornice_sala_sonar(Tela& t, const TemaStazione& tema, const FontConsolle& f) {
    const Rettangolo prima = t.ritaglio();
    const int W = LARGHEZZA_TELA, H = ALTEZZA_VISTA;
    t.imposta_ritaglio({0, 0, W, H});
    // la consolle della sala sonar (la luce blu la da' il tema, LuceSala::blu): il frontale attorno agli schermi, la fila
    // di tasti e lampade in alto, la lampada del WLR-9 (siluro in acqua)
    pannello(t, {0, 0, W, H}, tema.battello, 4);
    t.rettangolo_pieno(0, 0, W, 16, smalto(2));
    targhetta(t, {6, 3, 0, 0}, "AN/BQQ-5 SONAR", f.piccolo);
    for (int k = 0; k < 10; ++k) lampada_rettangolare(t, {150 + k * 30, 4, 22, 8}, k == 9 ? ColoreLampada::rossa : ColoreLampada::verde, k % 3 == 0);
    etichetta_sx(t, 452, 5, "WLR-9", f.piccolo, false);
    const Rettangolo libero{6, 20, W - 12, H - 26};
    t.rettangolo_pieno(libero.x - 1, libero.y - 1, libero.w + 2, libero.h + 2, static_cast<std::uint8_t>(colori_tema::ACCENTO));
    t.imposta_ritaglio(prima);
    return libero;
}

void cornice_sozh(Tela& t, Rettangolo carta, const TemaStazione& tema, const FontConsolle& f, double pennino_x, double pennino_y) {
    (void)tema;   // il Sozh e' solo dell'Alfa: lo smalto e' quello del tema applicato
    const Rettangolo prima = t.ritaglio();
    t.togli_ritaglio();
    // la cornice crema del tavolo attorno alla carta (8 pixel), la targhetta
    for (int k = 1; k <= 8; ++k) {
        const std::uint8_t c = smalto(k == 1 ? 6 : k == 8 ? 5 : 9);
        t.linea_orizzontale(carta.x - k, carta.x + carta.w - 1 + k, carta.y - k, c);
        t.linea_orizzontale(carta.x - k, carta.x + carta.w - 1 + k, carta.y + carta.h - 1 + k, c);
        t.linea_verticale(carta.x - k, carta.y - k, carta.y + carta.h - 1 + k, c);
        t.linea_verticale(carta.x + carta.w - 1 + k, carta.y - k, carta.y + carta.h - 1 + k, c);
    }
    // in basso a sinistra, a cavallo della cornice: in alto a sinistra c'e' l'intestazione della carta
    targhetta(t, {carta.x + 4, carta.y + carta.h - 4, 0, 0}, "SOZH", f.piccolo);
    // il carrello: sulla carta una traversa sottile, puntinata e chiara come la cornice (si vedono le X dei contatti sotto,
    // e non si confonde con la costa), i pattini pieni sulle rotaie ai due bordi, il pennino col suo cerchietto
    const int py = carta.y + static_cast<int>(std::lround(pennino_y)), px = carta.x + static_cast<int>(std::lround(pennino_x));
    for (const int xb : {carta.x - 8, carta.x + carta.w})   // i pattini, sulla cornice
        t.rettangolo_pieno(xb, py - 3, 8, 7, smalto(5));
    t.imposta_ritaglio(carta);
    for (int x = carta.x; x < carta.x + carta.w; x += 3) t.pixel(x, py, smalto(8));
    t.rettangolo_pieno(px - 4, py - 4, 9, 9, smalto(5));
    t.rettangolo_pieno(px - 2, py - 2, 5, 5, static_cast<std::uint8_t>(colori_tema::ACCENTO));
    t.pixel(px, py, lampada(ColoreLampada::rossa, true));
    t.imposta_ritaglio(prima);
}

}  // namespace m688
