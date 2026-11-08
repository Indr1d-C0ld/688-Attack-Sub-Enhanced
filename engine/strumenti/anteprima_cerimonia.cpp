// Anteprime della grafica attorno alla missione (M4, docs/12): medaglie e nastrini, ritratti, cornici, minicartoline,
// e le schermate di prova che li mettono insieme. Immagini senza finestra in extracted/navi/_verifica/cerimonia/.
// Uso: anteprima_cerimonia <dati> <cosa> <uscita.bmp>
//   cosa: medaglie, medaglie_urss, nastrini, ritratti, ritratto_grande, teste, cerimonia_usn, cerimonia_vmf, equipaggio,
//         giornale, fascicolo_elenco, icone, medaglioni (gli achievement in oro 48x48), medaglioni_stati (livelli,
//         non presi, nascosti, e la striscia 24x24 con la tavolozza della vista),
//         promozione, debriefing, fascicolo
//      anteprima_cerimonia <dati> manuale <cartella>   ogni decorazione da sola, per il manuale (volume 3)

#include <chrono>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <cstdlib>
#include <functional>
#include <string>
#include <vector>

#include "grafica/cartolina.hpp"
#include "grafica/cerimonia.hpp"
#include "grafica/cornici.hpp"
#include "grafica/effetti.hpp"
#include "grafica/periscopio.hpp"
#include "risorse.hpp"
#include "grafica/decorazioni.hpp"
#include "grafica/font.hpp"
#include "grafica/giornale.hpp"
#include "grafica/medaglioni.hpp"
#include "grafica/palette.hpp"
#include "grafica/ritratto.hpp"
#include "grafica/tela.hpp"
#include "nucleo/carriera.hpp"

using namespace m688;
using namespace m688::cerimonia;

namespace {

const TavolaDecorazioni* tav = nullptr;   // dalle regole della carriera e da decorazioni_grafica.toml

void sfondo(Tela& t, int h) {
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < 640; ++x) t.pixel(x, y, static_cast<std::uint8_t>(SFONDO + retina(1.0 + 2.5 * (1.0 - y / static_cast<double>(h)), x, y, 7)));
}


// testo a capo in una larghezza, riga per riga
int paragrafo(Tela& t, const Font& f, int x, int y, int w, const std::string& testo, int col, int interlinea = 11) {
    std::string riga, parola;
    auto emetti = [&]() {
        f.scrivi(t, x, y, riga, static_cast<std::uint8_t>(col), 1);
        y += interlinea;
        riga.clear();
    };
    for (std::size_t i = 0; i <= testo.size(); ++i) {
        const char c = i < testo.size() ? testo[i] : ' ';
        if (c != ' ') { parola += c; continue; }
        if (parola.empty()) continue;
        const std::string prova = riga.empty() ? parola : riga + " " + parola;
        if (f.larghezza(prova, 1) > w && !riga.empty()) { emetti(); riga = parola; }
        else riga = prova;
        parola.clear();
    }
    if (!riga.empty()) emetti();
    return y;
}

void tenda(Tela& t, int h) {   // fondale della cerimonia: tenda di velluto a pieghe
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < 640; ++x) {
            const double piega = 0.5 + 0.5 * std::sin(x * 0.09 + 0.6 * std::sin(x * 0.023));
            t.pixel(x, y, static_cast<std::uint8_t>(BANDIERA + retina(1.0 + 4.5 * piega * (0.75 + 0.25 * y / static_cast<double>(h)), x, y, 7)));
        }
}

void schermata_cerimonia(Tela& t, Palette& pal, const Font& font, bool usn) {
    ParametriRitratto r;
    r.marina = usn ? Marina::usn : Marina::vmf;
    r.comando = true;
    if (usn) {
        r.grado = 5;
        r.volto = {3, 42, {70, 45, 30}, false};
        r.nastrini = {{"navy_cross", "", 1}, {"legion_of_merit", "", 1}, {"meritorious_service", "", 2}, {"navy_commendation", "v", 1}, {"navy_achievement", "", 1}, {"navy_unit_commendation", "", 1}, {"navy_e_ribbon", "", 1}, {"navy_expeditionary", "", 3}, {"national_defense", "", 1}};
    } else {
        r.grado = 6;
        r.berretto = true;
        r.volto = {5, 40, {40, 32, 26}, false};
        r.volto.occhi = {110, 90, 60};
        r.nastrini = {{"stella_rossa", "", 1}, {"servizio_patria_3", "", 1}, {"distinzione_2", "", 1}, {"giubilare_70", "", 1}, {"servizio_impeccabile_3", "", 1}};
    }
    prepara_palette_ritratto(pal, r, tav);
    tenda(t, 480);
    intestazione(t, font, {0, 0, 640, 38}, usn ? "NAVY CROSS" : "ORDINE DELLA STELLA ROSSA",
                 usn ? "IL PRESIDENTE DEGLI STATI UNITI HA IL PIACERE DI CONFERIRE" : "DECRETO DEL PRESIDIUM DEL SOVIET SUPREMO DELL'URSS",
                 r.marina, false);
    // ritratto nella cornice
    cornice(t, {18, 50, 252, 326}, usn ? StileCornice::oro : StileCornice::rossa, 8);
    disegna_ritratto(t, {26, 58, 236, 310}, r, tav);
    // medaglia nell'astuccio di legno con il velluto
    const Rettangolo astuccio{300, 50, 150, 210};
    cornice(t, astuccio, StileCornice::legno, 9);
    for (int y = astuccio.y + 9; y < astuccio.y + astuccio.h - 9; ++y)
        for (int x = astuccio.x + 9; x < astuccio.x + astuccio.w - 9; ++x)
            t.pixel(x, y, static_cast<std::uint8_t>(VELLUTO + retina(2.0 + 3.5 * (1.0 - std::hypot(x - 375.0, y - 140.0) / 120.0) + 0.6 * caso(x, y, 9), x, y, 7)));
    tav->disegna_medaglia(t, {astuccio.x + 14, astuccio.y + 14, astuccio.w - 28, astuccio.h - 28}, usn ? "navy_cross" : "stella_rossa");
    // nastrino che si aggiungera' alla barretta
    font.scrivi(t, 470, 60, usn ? "NASTRINO" : "NASTRINO", static_cast<std::uint8_t>(ORO + 6), 1);
    tav->disegna_nastrino(t, 470, 76, 70, 20, VoceBarretta{usn ? "navy_cross" : "stella_rossa", "", 1});
    font.scrivi(t, 470, 106, usn ? "CAPITANO DI FREGATA" : "CAPITANO DI 2 RANGO", static_cast<std::uint8_t>(ORO + 6), 1);
    font.scrivi(t, 470, 118, usn ? "J. R. HALLORAN" : "V. A. SUCHKOV", 15, 1);
    font.scrivi(t, 470, 130, usn ? "USS DALLAS (SSN 700)" : "K-123, FLOTTA DEL NORD", static_cast<std::uint8_t>(BIANCO + 2), 1);
    // motivazione su carta
    const Rettangolo foglio{290, 270, 336, 168};
    carta(t, foglio, 7);
    const std::string mot = usn
        ? "Per lo straordinario eroismo dimostrato come comandante della USS Dallas (SSN 700) durante operazioni contro "
          "forze nemiche nel Mare di Norvegia. Con abile manovra eluse la scorta, si porto' in posizione di lancio e "
          "affondo' un incrociatore lanciamissili, riconducendo il battello senza danni. Il suo coraggio, il suo "
          "giudizio e la sua dedizione al dovere furono all'altezza delle piu' alte tradizioni della Marina degli Stati Uniti."
        : "Per il coraggio e l'audacia dimostrati nell'esecuzione di un incarico speciale del comando, decorare con "
          "l'Ordine della Stella Rossa il capitano di 2 rango Suchkov Viktor Andreevich, comandante dell'incrociatore "
          "subacqueo K-123. Mosca, Cremlino, 1989. Non per la stampa.";
    paragrafo(t, font, foglio.x + 12, foglio.y + 12, foglio.w - 24, mot, NERO + 1);
    timbro(t, font, foglio.x + foglio.w - 70, foglio.y + foglio.h - 28, usn ? "CLASSIFIED" : "CEKPETHO", -12, 2, 5);
    pulsante(t, font, {300, 448, 100, 22}, "CONTINUA", true);
    pulsante(t, font, {410, 448, 100, 22}, "MOTIVAZIONE", false);
    pulsante(t, font, {520, 448, 100, 22}, "FASCICOLO", false);
    pulsante(t, font, {18, 448, 120, 22}, usn ? "NASTRINI" : "NASTRINI", false);
}

}  // namespace

int main(int argc, char** argv) {
    if (argc < 4) {
        std::fprintf(stderr, "uso: %s <dati> <medaglie|nastrini> <uscita.bmp>\n", argv[0]);
        return 2;
    }
    const std::string dati = argv[1], cosa = argv[2], uscita = argv[3];
    auto pal = Palette::carica(dati + "/palette/giorno.pal");
    const auto regole = carica_carriera(dati + "/carriera.toml");
    const auto tavola = TavolaDecorazioni::carica(regole, dati + "/decorazioni_grafica.toml");
    tav = &tavola;
    const auto font = Font::carica(dati + "/font/originale.fnt");
    Tela t;
    t.riempi(0);
    int H = 480;
    if (cosa == "manuale") {   // ogni decorazione da sola (<uscita>/<id>.bmp, medaglia in 0,0-120x180), per il manuale
        std::filesystem::create_directories(uscita);
        for (const auto& d : tav->tutte()) {
            Palette p = pal;
            imposta_palette(p, d.marina);
            tav->imposta_palette(p);
            Tela m;
            m.riempi(0);
            sfondo(m, 180);
            tav->disegna_medaglia(m, {8, 8, 104, 164}, d.id);
            m.salva_bmp(uscita + "/" + d.id + ".bmp", p);
        }
        std::printf("%zu decorazioni in %s\n", tav->tutte().size(), uscita.c_str());
        return 0;
    }
    if (cosa == "medaglie" || cosa == "medaglie_urss") {
        const bool urss = cosa == "medaglie_urss";
        imposta_palette(pal, urss ? Marina::vmf : Marina::usn);
        tav->imposta_palette(pal);
        sfondo(t, H);
        int i = 0;
        for (const auto& d : tav->tutte()) {
            if ((d.marina == Marina::vmf) != urss || d.forma == FormaMedaglia::distintivo) continue;
            const int col = i % 7, riga = i / 7;
            const Rettangolo r{8 + col * 90, 8 + riga * 156, 84, 132};
            tav->disegna_medaglia(t, r, d.id);
            std::string n = d.nome;
            while (font.larghezza(n, 1) > 86 && n.size() > 3) n.pop_back();
            font.scrivi(t, r.x, r.y + r.h + 2, n, 15, 1);
            ++i;
        }
    } else if (cosa == "medaglioni") {
        imposta_palette(pal, Marina::usn);
        sfondo(t, H);
        const auto& ic = icone_medaglioni();
        for (std::size_t i = 0; i < ic.size(); ++i) {
            const int col = static_cast<int>(i % 10), riga = static_cast<int>(i / 10);
            const Rettangolo r{14 + col * 62, 6 + riga * 67, 48, 48};   // sette file da dieci: 68 medaglioni
            disegna_medaglione(t, r, ic[i], LivelloMedaglione::oro, true);
            std::string n = ic[i];
            while (font.larghezza(n, 1) > 60 && n.size() > 3) n.pop_back();
            font.scrivi(t, r.x - 4, r.y + 50, n, 15, 1);
            font.scrivi(t, r.x - 4, r.y + 59, categoria_medaglione(ic[i]), 7, 1);
        }
    } else if (cosa == "medaglioni_stati") {
        imposta_palette(pal, Marina::usn);
        sfondo(t, 330);
        const char* scelti[] = {"fantasma", "cacciatore", "orecchio_fino", "barents", "dolphins", "autore", "sasso"};
        int riga = 0;
        for (const char* id : scelti) {
            const int y = 8 + riga * 46;
            font.scrivi(t, 8, y + 18, id, 15, 1);
            int x = 120;
            for (const auto l : {LivelloMedaglione::bronzo, LivelloMedaglione::argento, LivelloMedaglione::oro}) {
                disegna_medaglione(t, {x, y, 40, 40}, id, l, true);
                x += 48;
            }
            disegna_medaglione(t, {x, y, 40, 40}, id, LivelloMedaglione::oro, false);
            x += 48;
            disegna_medaglione(t, {x, y, 40, 40}, id, LivelloMedaglione::oro, false, true);
            x += 60;
            // la striscia del debriefing: 24x24 con la tavolozza della vista, su fondo scuro
            t.rettangolo_pieno(x - 4, y + 6, 4 * 28 + 6, 30, 0);
            for (const auto l : {LivelloMedaglione::bronzo, LivelloMedaglione::argento, LivelloMedaglione::oro})
                disegna_medaglione(t, {x, y + 8, 24, 24}, id, l, true, false, TavolozzaMedaglione::vista), x += 28;
            disegna_medaglione(t, {x, y + 8, 24, 24}, id, LivelloMedaglione::oro, false, true, TavolozzaMedaglione::vista);
            ++riga;
        }
        font.scrivi(t, 120, 335, "BRONZO  ARGENTO  ORO   NON PRESO   NASCOSTO      STRISCIA 24X24 (VISTA)", 15, 1);
    } else if (cosa == "ritratti") {
        std::vector<ParametriRitratto> rr(6);
        rr[0].grado = 5; rr[0].comando = true; rr[0].volto = {3, 42, {70, 45, 30}, false};
        rr[0].nastrini = {{"legion_of_merit", "", 1}, {"meritorious_service", "", 2}, {"navy_commendation", "v", 1}, {"navy_achievement", "", 1}, {"navy_unit_commendation", "", 1}, {"navy_e_ribbon", "", 1}, {"navy_expeditionary", "", 3}, {"national_defense", "", 1}};
        rr[1] = rr[0]; rr[1].berretto = true; rr[1].volto.seme = 11;
        rr[2].grado = 4; rr[2].uniforme = Uniforme::kaki; rr[2].volto = {29, 36, {70, 45, 30}, false};
        rr[3].marina = Marina::vmf; rr[3].grado = 6; rr[3].berretto = true; rr[3].comando = true; rr[3].volto = {5, 40, {70, 45, 30}, false};
        rr[3].nastrini = {{"stella_rossa", "", 1}, {"servizio_patria_3", "", 1}, {"distinzione_2", "", 1}, {"giubilare_70", "", 1}, {"servizio_impeccabile_3", "", 1}};
        rr[4] = rr[3]; rr[4].berretto = false; rr[4].grado = 5; rr[4].volto = {17, 34, {70, 45, 30}, true};
        rr[5] = rr[0]; rr[5].grado = 6; rr[5].volto = {41, 55, {70, 45, 30}, false};
        rr[5].nastrini.insert(rr[5].nastrini.begin(), VoceBarretta{"navy_cross", "", 1});
        prepara_palette_ritratto(pal, rr[0], tav);
        for (int i = 0; i < 6; ++i) disegna_ritratto(t, {8 + (i % 3) * 212, 6 + (i / 3) * 238, 176, 232}, rr[static_cast<std::size_t>(i)], tav);
    } else if (cosa == "ritratto_grande") {
        ParametriRitratto r;
        r.grado = 5; r.comando = true; r.volto = {3, 42, {70, 45, 30}, false};
        r.nastrini = {{"legion_of_merit", "", 1}, {"meritorious_service", "", 2}, {"navy_commendation", "v", 1}, {"navy_achievement", "", 1}, {"navy_unit_commendation", "", 1}, {"navy_e_ribbon", "", 1}, {"navy_expeditionary", "", 3}, {"national_defense", "", 1}};
        prepara_palette_ritratto(pal, r, tav);
        disegna_ritratto(t, {0, 0, 352, 464}, r, tav);
        ParametriRitratto v = r;
        v.marina = Marina::vmf; v.grado = 6; v.berretto = true; v.volto = {5, 40, {70, 45, 30}, false};
        v.nastrini = {{"stella_rossa", "", 1}, {"servizio_patria_3", "", 1}, {"distinzione_2", "", 1}, {"giubilare_70", "", 1}, {"servizio_impeccabile_3", "", 1}};
        disegna_ritratto(t, {320, 0, 352, 464}, v, tav);
    } else if (cosa == "teste") {
        // prova dei lineamenti: di fronte, di tre quarti e di profilo, tre volti
        ParametriRitratto r;
        r.solo_testa = true;
        prepara_palette_ritratto(pal, r, tav);
        for (int k = 0; k < 2; ++k)
            for (int j = 0; j < 3; ++j) {
                r.volto.seme = static_cast<std::uint32_t>(3 + 14 * k);
                r.girata_gradi = j == 0 ? 0 : j == 1 ? -35 : -90;
                disegna_ritratto(t, {j * 213 - 20, k * 240 - 10, 240, 316}, r, tav);
            }

    } else if (cosa == "cerimonia_usn" || cosa == "cerimonia_vmf") {
        schermata_cerimonia(t, pal, font, cosa == "cerimonia_usn");
    } else if (cosa == "promozione") {
        ParametriRitratto r;
        r.grado = 6;
        r.volto = {3, 47, {70, 45, 30}, false};
        r.nastrini = {{"navy_cross", "", 1}, {"legion_of_merit", "", 1}, {"meritorious_service", "", 2}, {"navy_commendation", "v", 1}, {"navy_achievement", "", 1}, {"navy_unit_commendation", "", 1}, {"navy_e_ribbon", "", 1}, {"navy_expeditionary", "", 3}, {"national_defense", "", 1}};
        r.comando = true;
        r.berretto = true;
        prepara_palette_ritratto(pal, r, tav);
        tenda(t, 480);
        intestazione(t, font, {0, 0, 640, 38}, "PROMOZIONE", "IL SEGRETARIO DELLA MARINA", Marina::usn, false);
        cornice(t, {18, 50, 252, 326}, StileCornice::oro, 8);
        disegna_ritratto(t, {26, 58, 236, 310}, r, tav);
        cornice(t, {300, 60, 150, 96}, StileCornice::console, 5);
        disegna_insegna_grado(t, {305, 65, 140, 86}, Marina::usn, 5);
        cornice(t, {470, 60, 150, 96}, StileCornice::oro, 5);
        disegna_insegna_grado(t, {475, 65, 140, 86}, Marina::usn, 6);
        font.scrivi(t, 300, 162, "CAPITANO DI FREGATA", static_cast<std::uint8_t>(BIANCO + 2), 1);
        font.scrivi(t, 470, 162, "CAPITANO DI VASCELLO", static_cast<std::uint8_t>(ORO + 7), 1);
        for (int k = 0; k < 10; ++k) t.pixel(455 + k, 108, static_cast<std::uint8_t>(ORO + 6));
        cornice(t, {300, 190, 150, 96}, StileCornice::console, 5);
        disegna_insegna_grado(t, {305, 195, 140, 86}, Marina::vmf, 6);
        cornice(t, {470, 190, 150, 96}, StileCornice::rossa, 5);
        disegna_insegna_grado(t, {475, 195, 140, 86}, Marina::vmf, 7);
        font.scrivi(t, 300, 292, "CAPITANO DI 2 RANGO", static_cast<std::uint8_t>(BIANCO + 2), 1);
        font.scrivi(t, 470, 292, "CAPITANO DI 1 RANGO", static_cast<std::uint8_t>(ORO + 7), 1);
        pulsante(t, font, {520, 448, 100, 22}, "CONTINUA", true);
    } else if (cosa == "fascicolo") {
        ParametriRitratto r;
        r.grado = 5;
        r.volto = {29, 44, {150, 120, 70}, false};
        r.volto.occhi = {70, 110, 80};
        r.nastrini = {{"legion_of_merit", "", 1}, {"meritorious_service", "", 2}, {"navy_commendation", "v", 1}, {"navy_achievement", "", 1}, {"navy_unit_commendation", "", 1}, {"navy_e_ribbon", "", 1}, {"navy_expeditionary", "", 3}, {"national_defense", "", 1}};
        r.comando = true;
        prepara_palette_ritratto(pal, r, tav);
        for (int y = 0; y < 480; ++y)
            for (int x = 0; x < 640; ++x) t.pixel(x, y, static_cast<std::uint8_t>(LEGNO + retina(2.2 + 1.2 * std::sin((x + 9 * std::sin(y * 0.05)) * 0.05) + 0.5 * caso(x / 5, y, 2), x, y, 7)));
        carta(t, {40, 20, 560, 450}, 11);
        timbro(t, font, 500, 36, "CONFIDENTIAL", -6, 2, 3);
        font.scrivi(t, 64, 40, "OFFICER SERVICE RECORD", static_cast<std::uint8_t>(NERO + 1), 2);
        font.scrivi(t, 64, 64, "FASCICOLO PERSONALE DELL'UFFICIALE", static_cast<std::uint8_t>(NERO + 2), 1);
        // foto con la graffetta
        cornice(t, {62, 92, 140, 182}, StileCornice::console, 3);
        disegna_ritratto(t, {65, 95, 134, 176}, r, tav);
        for (int y = 86; y < 112; ++y) { t.pixel(150, y, static_cast<std::uint8_t>(ARGENTO + 4)); t.pixel(156, y, static_cast<std::uint8_t>(ARGENTO + 2)); }
        int y = 96;
        for (const char* riga : {"NOME: HALLORAN, JAMES R.", "GRADO: CAPITANO DI FREGATA (O-5)", "PROVENIENZA: U.S. NAVAL ACADEMY, 1972",
                                 "QUALIFICHE: SOMMERGIBILI 1974, INGEGNERE 1978,", "   COMANDO 1985", "INCARICO: COMANDANTE, USS DALLAS (SSN 700)",
                                 "MISSIONI: 14   PUNTEGGIO MEDIO: 812"}) {
            font.scrivi(t, 220, y, riga, static_cast<std::uint8_t>(NERO + 1), 1);
            y += 14;
        }
        font.scrivi(t, 64, 300, "DECORAZIONI", static_cast<std::uint8_t>(NERO + 1), 1);
        tav->disegna_barretta(t, 160, 352, r.nastrini, 3, 35, 10);
        y = 300;
        for (const char* riga : {"LEGION OF MERIT, 1988", "MERITORIOUS SERVICE MEDAL (2), 1985, 1987", "NAVY COMMENDATION MEDAL CON V, 1983",
                                 "NAVY ACHIEVEMENT MEDAL, 1979", "NAVY UNIT COMMENDATION, 1987", "NAVY E RIBBON, 1986"}) {
            font.scrivi(t, 260, y, riga, static_cast<std::uint8_t>(NERO + 2), 1);
            y += 14;
        }
        font.scrivi(t, 64, 400, "NOTE CARATTERISTICHE: ECCELLENTE.", static_cast<std::uint8_t>(NERO + 1), 1);
        font.scrivi(t, 64, 414, "PROMOZIONE ANTICIPATA RACCOMANDATA.", static_cast<std::uint8_t>(NERO + 1), 1);
        timbro(t, font, 520, 420, "APPROVED", 6, 2, 9);
    } else if (cosa == "debriefing") {
        // minicartoline dal mondo ricostruito (palette della vista) e icone d'epoca
        const auto ris = carica_risorse(dati);
        pal = ris.giorno;
        Periscopio per(ris.font, ris.classi, ris.sprite);
        per.usa_modelli(ris.modelli, rampa_grigi(ris.giorno));
        for (int yy = 0; yy < 480; ++yy)
            for (int x = 0; x < 640; ++x) t.pixel(x, yy, static_cast<std::uint8_t>(144 + retina(2.2 + 0.8 * caso(x / 2, yy / 2, 3), x, yy, 4)));
        intestazione(t, font, {0, 0, 640, 38}, "DEBRIEFING", "MISSIONE 2: MARE DI NORVEGIA, 14 OTTOBRE 1989", Marina::usn, true);
        auto scheda = [&](int i, const std::string& didascalia, const std::function<void(Rettangolo)>& disegna) {
            const int col = i % 4, riga = i / 4;
            const Rettangolo f{14 + col * 156, 52 + riga * 160, 140, 108};
            cornice(t, f, StileCornice::console, 6, true);
            const Rettangolo r{f.x + 6, f.y + 6, 128, 96};
            disegna(r);
            paragrafo(t, font, f.x, f.y + f.h + 4, 146, didascalia, 15, 10);
        };
        auto lunghezza = [&](const std::string& c) { auto it = ris.classi.find(c); return it == ris.classi.end() ? 120.0 : it->second.lunghezza_m; };
        // 1. siluro a segno: colonna d'acqua sotto una Krivak II
        {
            Mondo m;
            Nave n; n.id = 1; n.classe = "krivak2"; n.pos = {0, 0}; n.rotta = 60; n.velocita = 0;
            m.aggiungi(n);
            Effetti e(3);
            e.esplosione_subacquea({0, 0}, 4.0, 300.0);
            for (int k = 0; k < 30; ++k) e.avanza(0.05, m);
            auto q = inquadra_nave(m.navi()[0], lunghezza("krivak2"), 1.5, 90);
            scheda(0, "00:42 SILURO A SEGNO SULLA KRIVAK II", [&](Rettangolo r) { disegna_cartolina(t, r, per, m, q, &e); });
        }
        // 2. affondamento: la nave appruata e sbandata, fumo e fuoco
        {
            Mondo m;
            Nave n; n.id = 1; n.classe = "kara"; n.pos = {0, 0}; n.rotta = 300;
            n.posa.danneggiata = true; n.posa.prodiero.assetto = -9; n.posa.prodiero.sbandamento = 14; n.posa.prodiero.immersione = 5;
            m.aggiungi(n);
            Effetti e(5);
            e.esplosione_aerea({20, 0}, 6.0, 80.0, true);
            for (int k = 0; k < 200; ++k) e.avanza(0.1, m);
            auto q = inquadra_nave(m.navi()[0], lunghezza("kara"), 20.0, 90);
            scheda(1, "00:51 KARA IN AFFONDAMENTO", [&](Rettangolo r) { disegna_cartolina(t, r, per, m, q, &e); });
        }
        // 3. emersione violenta di un Victor III
        {
            Mondo m;
            Nave n; n.id = 1; n.classe = "victor3"; n.pos = {0, 0}; n.rotta = 30;
            n.posa.danneggiata = true; n.posa.prodiero.assetto = 24; n.posa.prodiero.immersione = 3; n.posa.prodiero.x_perno = -20;
            m.aggiungi(n);
            Effetti e(7);
            e.emersione_violenta({0, 0}, 30, 3.0, 24, lunghezza("victor3"), 0.3, 3.0);
            for (int k = 0; k < 40; ++k) e.avanza(0.05, m);
            auto q = inquadra_nave(m.navi()[0], lunghezza("victor3"), 2.0, 90);
            scheda(2, "01:07 EMERSIONE D'EMERGENZA DI UN VICTOR III", [&](Rettangolo r) { disegna_cartolina(t, r, per, m, q, &e); });
        }
        // 4. lancio di missili da un Oscar in immersione
        {
            Mondo m;
            Nave n; n.id = 1; n.classe = "oscar"; n.pos = {0, 0}; n.rotta = 90; n.quota = 25;
            m.aggiungi(n);
            Effetti e(9);
            const double tm = e.lancio_missile({0, 0}, 90, 25, "granit");
            for (double k = 0; k < tm + 2.5; k += 0.05) e.avanza(0.05, m);
            auto q = inquadra({0, 0}, 2600, 180, tm + 2.5);
            q.alzo = 0.3;
            scheda(3, "01:33 LANCIO DI SS-N-19 DA UN OSCAR", [&](Rettangolo r) { disegna_cartolina(t, r, per, m, q, &e); });
        }
        scheda(4, "00:12 CONTATTO SIERRA 1 SULLA CASCATA", [&](Rettangolo r) { disegna_icona(t, font, r, Icona::cascata, 3.0, 2); });
        scheda(5, "00:38 PING DI CONFERMA, DISTANZA 4200 YD", [&](Rettangolo r) { disegna_icona(t, font, r, Icona::sonar, 2.2, 3); });
        scheda(6, "01:15 MESSAGGIO FLASH DA COMSUBLANT", [&](Rettangolo r) { disegna_icona(t, font, r, Icona::telescrivente, 9.0, 4, "FLASH. PROCEDERE A SETTORE BRAVO. ROE INVARIATE. CONFERMARE."); });
        scheda(7, "01:40 RADAR TOP SAIL, RISCHIO ALTO", [&](Rettangolo r) { disegna_icona(t, font, r, Icona::esm, 0.2, 5); });
        timbro(t, font, 560, 440, "CLASSIFIED", -10, 2, 2, true);
    } else if (cosa == "equipaggio") {
        // ritratti di gruppo: USN nel riquadro del ritratto, VMF piu' largo con feriti e caduti
        ParametriRitratto r;
        prepara_palette_ritratto(pal, r, tav);
        for (int y = 0; y < 480; ++y)
            for (int x = 0; x < 640; ++x) t.pixel(x, y, static_cast<std::uint8_t>(BANDIERA + retina(2.0, x, y, 7)));
        ParametriEquipaggio u;
        u.battello = "700";
        cornice(t, {8, 8, 252, 326}, StileCornice::oro, 8);
        disegna_equipaggio(t, font, {16, 16, 236, 310}, u);
        ParametriEquipaggio v;
        v.marina = Marina::vmf;
        v.battello = "123";
        v.uomini = 22;
        v.caduti = 3;
        v.feriti = 2;
        v.grado_comandante = 6;
        cornice(t, {270, 8, 362, 252}, StileCornice::rossa, 8);
        disegna_equipaggio(t, font, {278, 16, 346, 236}, v);
        ParametriEquipaggio w = u;
        w.uomini = 14;
        w.feriti = 3;
        w.caduti = 1;
        cornice(t, {270, 270, 362, 202}, StileCornice::oro, 6);
        disegna_equipaggio(t, font, {276, 276, 350, 190}, w);
    } else if (cosa == "giornale") {
        // esiti narrativi: protesta diplomatica (USN) e battello in ritardo (comunicato TASS)
        ParametriRitratto r;
        prepara_palette_ritratto(pal, r, tav);
        for (int y = 0; y < 480; ++y)
            for (int x = 0; x < 640; ++x) t.pixel(x, y, static_cast<std::uint8_t>(LEGNO + retina(2.2 + 1.2 * std::sin((x + 9 * std::sin(y * 0.05)) * 0.05) + 0.5 * caso(x / 5, y, 2), x, y, 7)));
        disegna_giornale(t, font, {10, 10, 400, 460}, Marina::usn, "The Washington Herald", "Norway protests: fishing trawler sunk off the Faroes",
                         "Oslo ha convocato ieri l'ambasciatore americano dopo l'affondamento del peschereccio Hansine, colpito da un siluro "
                         "nelle acque a nord delle isole Faroe durante un'esercitazione della NATO. Il Pentagono non conferma la presenza di "
                         "sommergibili americani nella zona. Il governo norvegese chiede un'inchiesta e la sospensione delle operazioni "
                         "navali alleate nel Mare di Norvegia fino alla fine delle indagini.",
                         "Washington, sabato 14 ottobre 1989", 3);
        disegna_giornale(t, font, {418, 10, 212, 460}, Marina::vmf, "Pravda", "Il sommergibile K-123 non e' rientrato alla base",
                         "Il sommergibile nucleare K-123 della Flotta del Nord, impegnato in un'esercitazione nel Mare di Barents, non ha "
                         "fatto rientro alla base all'ora prevista. Sono in corso le ricerche con navi e aerei della flotta. Il comando "
                         "esprime fiducia nell'esito delle operazioni di soccorso.",
                         "Mosca, 15 ottobre 1989", 4);
    } else if (cosa == "fascicolo_elenco") {
        ParametriRitratto r;
        r.grado = 5;
        r.comando = true;
        r.volto = {29, 44, {150, 120, 70}, false};
        r.nastrini = {{"navy_cross", "", 1}, {"legion_of_merit", "", 1}, {"meritorious_service", "", 2}, {"navy_commendation", "v", 1},
                      {"navy_unit_commendation", "", 1}, {"navy_e_ribbon", "", 1}, {"sea_service_deployment", "", 3}, {"national_defense", "", 1}};
        prepara_palette_ritratto(pal, r, tav);
        for (int y = 0; y < 480; ++y)
            for (int x = 0; x < 640; ++x) t.pixel(x, y, static_cast<std::uint8_t>(LEGNO + retina(2.2 + 1.2 * std::sin((x + 9 * std::sin(y * 0.05)) * 0.05) + 0.5 * caso(x / 5, y, 2), x, y, 7)));
        carta(t, {30, 14, 580, 456}, 11);
        timbro(t, font, 520, 34, "CONFIDENTIAL", -6, 2, 3);
        font.scrivi(t, 52, 30, "OFFICER SERVICE RECORD", static_cast<std::uint8_t>(NERO + 1), 2);
        cornice(t, {50, 60, 120, 156}, StileCornice::console, 3);
        disegna_ritratto(t, {53, 63, 114, 150}, r, tav);
        int y = 66;
        for (const char* riga : {"HALLORAN, JAMES R.  CDR, USN", "COMANDANTE, USS DALLAS (SSN 700)", "MISSIONI 14, MEDIA 812, GRADUATORIA 3/41"}) {
            font.scrivi(t, 186, y, riga, static_cast<std::uint8_t>(NERO + 1), 1);
            y += 14;
        }
        font.scrivi(t, 186, 120, "DECORAZIONI", static_cast<std::uint8_t>(NERO + 1), 1);
        tav->disegna_barretta(t, 300, 212, r.nastrini, 3, 35, 10);
        disegna_tabella(t, font, {50, 232, 540, 220}, {{"data", 1.0}, {"evento", 2.4}, {"classe", 1.2}, {"teatro", 1.4}, {"esito", 1.0}},
                        {{"12.03.1988", "affondato", "krivak ii", "mare di norvegia", "missione 2"},
                         {"12.03.1988", "classificato", "victor iii", "mare di norvegia", "missione 2"},
                         {"02.07.1988", "affondato", "kara", "gibilterra", "missione 5"},
                         {"02.07.1988", "affondato", "nanuchka", "gibilterra", "missione 5"},
                         {"19.11.1988", "classificato", "alfa", "barents", "missione 3"},
                         {"03.02.1989", "affondato", "kilo", "otranto", "missione 6"},
                         {"14.10.1989", "affondato", "slava", "kunashir", "missione 7"}},
                        "affondamenti e contatti classificati");
    } else if (cosa == "asw" || cosa == "asw_notte") {
        // effetti dell'ASW aereo (docs/13 A4) nella vista a 6x, ritagliata 1:1: hover, cavo, MAD, boe, paracadute,
        // marcatori; asw_notte: luci e marcatori con la palette della notte
        const auto ris = carica_risorse(dati);
        const bool notte = cosa == "asw_notte";
        pal = notte ? ris.giorno.illuminata(0.08, INDICI_VISTA) : ris.giorno;
        Periscopio per(ris.font, ris.classi, ris.sprite);
        per.usa_modelli(ris.modelli, rampa_grigi(ris.giorno));
        std::string classe_volo;   // l'elicottero della scena, in volo (quota negativa)
        double quota_volo = 0, rotta_volo = 0;
        auto scena = [&](int i, const char* titolo, double secondi, double distanza, double alzo,
                         const std::function<void(Effetti&, double)>& prepara) {
            Mondo m;
            if (!classe_volo.empty()) {
                Nave n;
                n.id = 50; n.classe = classe_volo; n.pos = {0, 0}; n.rotta = rotta_volo; n.quota = -quota_volo;
                m.aggiungi(n);
            }
            Effetti e(11 + static_cast<std::uint64_t>(i));
            const double dt = 0.1;
            for (double tt = 0; tt < secondi - 1e-9; tt += dt) {
                prepara(e, tt);
                e.avanza(dt, m);
            }
            const auto q = inquadra({0, 0}, distanza, 200.0, secondi);
            Nave occhio;
            occhio.id = -1000000;
            occhio.pos = q.occhio;
            occhio.rotta = q.rilevamento;
            VistaPeriscopio v;
            v.rilevamento = q.rilevamento + 1.0;   // il punto a sinistra del reticolo
            v.alto_ingrandimento = true;
            v.quota_occhio_m = q.quota_occhio_m;
            v.luce = notte ? 0.08 : 1.0;
            v.tempo_s = secondi;
            static Tela appoggio;
            appoggio.riempi(0);
            per.disegna(appoggio, m, occhio, v, &e);
            const Rettangolo r{8 + (i % 2) * 316, 14 + (i / 2) * 232, 308, 210};
            const int x0 = (LARGHEZZA_TELA - r.w) / 2 - 60;
            const int y0 = std::clamp(static_cast<int>(Periscopio::riga_orizzonte(v) - r.h * alzo), 0, ALTEZZA_VISTA - r.h);
            for (int y = 0; y < r.h; ++y)
                for (int x = 0; x < r.w; ++x) t.pixel(r.x + x, r.y + y, appoggio.at(x0 + x, y0 + y));
            font.scrivi(t, r.x, r.y - 10, titolo, 15, 1);
        };
        AereoAsw sh3h;
        sh3h.id = 1; sh3h.elicottero = true; sh3h.hover = true; sh3h.quota_m = 12; sh3h.rotore_m = 18.9; sh3h.apertura_m = 4;
        sh3h.luci = notte;
        if (!notte) {
            classe_volo = "sh3h"; quota_volo = 12; rotta_volo = 110;
            scena(0, "SH-3H IN HOVER A 12 M: CAVO IN ACQUA", 8, 420, 0.8, [&](Effetti& e, double tt) {
                sh3h.cavo_m = std::min(90.0, 3.0 * tt + 60.0);
                e.aerei_asw({sh3h});
            });
            classe_volo = "ka27"; quota_volo = 30;
            scena(1, "KA-27 A 30 M: IL TRASDUTTORE SCENDE", 8, 900, 0.85, [&](Effetti& e, double tt) {
                AereoAsw a = sh3h;
                a.id = 2; a.quota_m = 30; a.rotore_m = 15.9; a.cavo_m = 1.5 * tt;
                e.aerei_asw({a});
            });
            std::vector<Sonoboa> boe;
            for (int k = 0; k < 4; ++k) {
                Sonoboa b;
                b.id = 1000 + k; b.tipo = k == 1 ? "rgb2" : "ssq53b";
                b.pos = {-6.0 + 4.0 * k, 3.0 * (k % 2)};
                b.t_posa = k == 3 ? 62.7 : 5.0 + 15.0 * k; b.t_impatto = b.t_posa + 3.0; b.t_attiva = b.t_impatto + 120; b.t_fine = 1e9;
                boe.push_back(b);
            }
            classe_volo.clear();
            scena(2, "BOE: GALLEGGIANTI, COLORANTE, UNA CHE SCENDE", 65.6, 260, 0.8, [&](Effetti& e, double tt) { e.boe_asw(boe, tt); });
            scena(3, "MK 46 COL PARACADUTE", 2.7, 420, 0.8, [&](Effetti& e, double tt) {
                if (tt == 0) e.arma_paracadute({0, 0}, 46, 20, 51, false);
            });
        } else {
            classe_volo = "sh3h"; quota_volo = 12; rotta_volo = 110;
            scena(0, "SH-3H IN HOVER, LUCI ACCESE", 6, 600, 0.8, [&](Effetti& e, double tt) {
                sh3h.cavo_m = 40 + tt;
                e.aerei_asw({sh3h});
            });
            classe_volo.clear();
            scena(1, "OMAB-25-8N E MK 58", 20, 500, 0.6, [&](Effetti& e, double tt) {
                if (tt == 0) {
                    e.marcatore_asw(MarcatoreAsw::omab_notte, {-6, 0}, 600);
                    e.marcatore_asw(MarcatoreAsw::mk58, {6, 0}, 3000);
                }
            });
        }
        if (!notte) {
            // seconda pagina: viene sostituita dalle scene di giorno coi marcatori (asw2)
        }
    } else if (cosa == "asw2") {
        const auto ris = carica_risorse(dati);
        pal = ris.giorno;
        Periscopio per(ris.font, ris.classi, ris.sprite);
        per.usa_modelli(ris.modelli, rampa_grigi(ris.giorno));
        auto scena = [&](int i, const char* titolo, double secondi, double distanza, double alzo,
                         const std::function<void(Effetti&, double)>& prepara) {
            Mondo m;
            Effetti e(21 + static_cast<std::uint64_t>(i));
            for (double tt = 0; tt < secondi - 1e-9; tt += 0.1) {
                prepara(e, tt);
                e.avanza(0.1, m);
            }
            const auto q = inquadra({0, 0}, distanza, 200.0, secondi);
            Nave occhio;
            occhio.id = -1000000; occhio.pos = q.occhio; occhio.rotta = q.rilevamento;
            VistaPeriscopio v;
            v.rilevamento = q.rilevamento + 1.0;
            v.alto_ingrandimento = true;
            v.quota_occhio_m = q.quota_occhio_m;
            v.tempo_s = secondi;
            static Tela appoggio;
            appoggio.riempi(0);
            per.disegna(appoggio, m, occhio, v, &e);
            const Rettangolo r{8 + (i % 2) * 316, 14 + (i / 2) * 232, 308, 210};
            const int x0 = (LARGHEZZA_TELA - r.w) / 2 - 60;
            const int y0 = std::clamp(static_cast<int>(Periscopio::riga_orizzonte(v) - r.h * alzo), 0, ALTEZZA_VISTA - r.h);
            for (int y = 0; y < r.h; ++y)
                for (int x = 0; x < r.w; ++x) t.pixel(r.x + x, r.y + y, appoggio.at(x0 + x, y0 + y));
            font.scrivi(t, r.x, r.y - 10, titolo, 15, 1);
        };
        scena(0, "SH-60B COL MAD A 60 M E 40 M/S", 3, 1600, 0.85, [&](Effetti& e, double tt) {
            AereoAsw a;
            a.id = 3; a.elicottero = true; a.quota_m = 60; a.velocita = 40; a.rotta = 290; a.mad_m = 55; a.apertura_m = 3;
            a.pos = {-40.0 * std::sin(radianti(290)) * (3 - tt), -40.0 * std::cos(radianti(290)) * (3 - tt)};
            e.aerei_asw({a});
        });
        scena(1, "MK 25 E MK 58 DOPO UN MINUTO", 60, 700, 0.75, [&](Effetti& e, double tt) {
            if (tt == 0) {
                e.marcatore_asw(MarcatoreAsw::mk25, {-8, 0}, 900);
                e.marcatore_asw(MarcatoreAsw::mk58, {8, 0}, 3000);
            }
        });
        scena(2, "OMAB-25-12D: FLUORESCEINA", 120, 300, 0.5, [&](Effetti& e, double tt) {
            if (tt == 0) e.marcatore_asw(MarcatoreAsw::omab_giorno, {0, 0}, 900);
        });
        scena(3, "MK 46: IL TONFO E IL PARACADUTE IN ACQUA", 4.0, 420, 0.6, [&](Effetti& e, double tt) {
            if (tt == 0) e.arma_paracadute({0, 0}, 46, 20, 51, false);
        });
    } else if (cosa == "icone") {
        // tutte le icone d'epoca delle minicartoline (palette della vista)
        const Font& fv = font;
        for (int y = 0; y < 480; ++y)
            for (int x = 0; x < 640; ++x) t.pixel(x, y, 144 + 3);
        const Icona tutte[] = {Icona::cascata, Icona::sonar, Icona::telescrivente, Icona::esm, Icona::radiogoniometro, Icona::danni, Icona::zona, Icona::mina,
                               Icona::aereo, Icona::boa, Icona::elicottero, Icona::marcatore};
        const char* nomi[] = {"cascata", "sonar", "radio", "esm", "radiogoniometro", "danni", "zona", "mina", "aereo", "boa", "elicottero", "marcatore"};
        for (int i = 0; i < 12; ++i) {
            const Rettangolo f{14 + (i % 4) * 156, 20 + (i / 4) * 152, 140, 108};
            cornice(t, f, StileCornice::console, 6, true);
            disegna_icona(t, fv, {f.x + 6, f.y + 6, 128, 96}, tutte[i], 1.3, 5, "FLASH. PROCEDERE A SETTORE BRAVO.");
            fv.scrivi(t, f.x, f.y + f.h + 4, nomi[i], 15, 1);
        }
    } else if (cosa == "tempo") {
        // costo di un ritratto grande (236 x 310) e piccolo (176 x 232)
        ParametriRitratto r;
        r.comando = true;
        r.berretto = true;
        r.nastrini = {{"legion_of_merit", "", 1}, {"meritorious_service", "", 2}};
        prepara_palette_ritratto(pal, r, tav);
        for (const Rettangolo q : {Rettangolo{0, 0, 236, 310}, Rettangolo{300, 0, 176, 232}}) {
            const auto a = std::chrono::steady_clock::now();
            disegna_ritratto(t, q, r, tav);
            const auto b = std::chrono::steady_clock::now();
            std::printf("ritratto %dx%d: %.0f ms\n", q.w, q.h, std::chrono::duration<double, std::milli>(b - a).count());
        }
    } else if (cosa == "nastrini") {
        imposta_palette(pal, Marina::usn);
        tav->imposta_palette(pal);
        sfondo(t, H);
        int y = 10;
        for (const bool urss : {false, true}) {
            int x = 10;
            for (const auto& d : tav->tutte()) {
                if ((d.marina == Marina::vmf) != urss) continue;
                tav->disegna_nastrino(t, x, y, urss ? 48 : 70, urss ? 16 : 20, VoceBarretta{d.id, "", 1});
                x += urss ? 52 : 74;
                if (x > 560) { x = 10; y += 26; }
            }
            y += 34;
        }
        // barrette d'esempio sulla scala del ritratto
        tav->disegna_barretta(t, 160, 320, {{"legion_of_merit", "", 1}, {"meritorious_service", "", 2}, {"navy_commendation", "v", 1}, {"navy_achievement", "", 1},
                                       {"navy_unit_commendation", "", 1}, {"navy_e_ribbon", "", 1}, {"navy_expeditionary", "", 3}, {"national_defense", "", 1}, {"sea_service_deployment", "", 4}}, 3, 14, 4);
        tav->disegna_barretta(t, 320, 320, {{"stella_rossa", "", 1}, {"servizio_patria_3", "", 1}, {"distinzione_2", "", 1}, {"giubilare_70", "", 1},
                                       {"servizio_impeccabile_3", "", 1}}, 4, 10, 4);
        tav->disegna_barretta(t, 480, 420, {{"legion_of_merit", "", 1}, {"meritorious_service", "", 2}, {"navy_commendation", "v", 1}, {"navy_achievement", "", 1},
                                       {"navy_unit_commendation", "", 1}, {"navy_e_ribbon", "", 1}, {"navy_expeditionary", "", 3}}, 3, 35, 10);
    }
    Tela fuori(640, H);
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < 640; ++x) fuori.pixel(x, y, t.at(x, y));
    fuori.salva_bmp(uscita, pal);
    return 0;
}
