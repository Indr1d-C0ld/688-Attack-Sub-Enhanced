// Anteprima delle sagome delle armi e della sezione del tubo (grafica/sagome_armi.hpp): tutte le armi di carico.toml
// in scala comune, di guerra e d'esercitazione, i tubi negli stati della missione e le stesse cose nei riquadri
// piccoli del pannello Armi con la tavolozza fissa; esche: le esche semoventi da tubo (MG-74, MOSS Mk 70) in tubo,
// nei riquadri del pannello e in grande accanto alle esche dei lanciatori (ADC Mk 2, MG-84).
#include <cstdio>
#include <string>

#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/sagome_armi.hpp"
#include "grafica/tela.hpp"
#include "nucleo/carico.hpp"

using namespace m688;

int main(int argc, char** argv) {
    if (argc < 4) {
        std::fprintf(stderr, "uso: %s <dati> <armi|esercitazione|senza_carico|tubi|pannello|esche> <uscita.bmp>\n", argv[0]);
        return 2;
    }
    const std::string dati = argv[1], cosa = argv[2], uscita = argv[3];
    auto pal = Palette::carica(dati + "/palette/giorno.pal");
    const auto font = Font::carica(dati + "/font/originale.fnt");
    const auto cat = carica_carico(dati + "/carico.toml");
    Tela t;
    t.riempi(0);

    if (cosa == "armi" || cosa == "esercitazione") {
        // ricca: tutte le armi alla stessa scala (8,2 m = 300 px), di guerra a sinistra e d'esercitazione a destra
        const auto tav = TavolozzaSagome::ricca(pal);
        const double px_m = 36.0;
        int i = 0;
        for (const auto& [id, a] : cat.armi) {
            const int col = i / 11, riga = i % 11;
            const int x = 8 + col * 316, y = 6 + riga * 43;
            font.scrivi(t, x, y, a.nome, colori::GRIGIO, 1);
            const SagomaArma s = sagoma_da_scheda(a, cosa == "esercitazione");
            const double L = lunghezza_disegnata(s);
            disegna_sagoma_arma(t, s, {x, y + 9, static_cast<int>(L * px_m) + 2, 32}, px_m, tav);
            ++i;
        }
    } else if (cosa == "senza_carico") {
        // sagoma_da_id: i siluri senza voce nel carico e le classi delle missioni, in scala comune (la balena a parte)
        const auto tav = TavolozzaSagome::ricca(pal);
        const char* ids[] = {"tigerfish", "spearfish", "mk46", "at1", "at2", "umgt1", "apr2", "type40", "type53", "mk48", "sea_lance"};
        const double px_m = 40.0;
        int i = 0;
        for (const char* id : ids) {
            for (const bool es : {false, true}) {
                const int x = 8 + es * 316, y = 4 + i * 36;
                font.scrivi(t, x, y, std::string(id) + (es ? " (esercitazione)" : ""), colori::GRIGIO, 1);
                const SagomaArma s = sagoma_da_id(id, es);
                disegna_sagoma_arma(t, s, {x, y + 8, static_cast<int>(lunghezza_disegnata(s) * px_m) + 2, 26}, px_m, tav);
            }
            ++i;
        }
        font.scrivi(t, 8, 404, "biologico (balenottera comune, 21 m)", colori::GRIGIO, 1);
        disegna_sagoma_arma(t, sagoma_da_id("biologico"), {8, 414, 620, 62}, 0.0, tav);
    } else if (cosa == "tubi") {
        const auto tav = TavolozzaSagome::ricca(pal);
        struct Caso {
            const char* id;
            FaseTubo stato;
            double ricarica;
            bool allagato, porta;
            const char* testo;
        };
        const Caso casi[] = {
            {"", FaseTubo::vuoto, 1, false, false, "VUOTO"},
            {"mk48_adcap", FaseTubo::ricarica, 0.35, false, false, "RICARICA 35%"},
            {"mk48_adcap", FaseTubo::carico, 1, false, false, "CARICO"},
            {"mk48_adcap", FaseTubo::armamento, 1, true, false, "ARMAMENTO: ALLAGATO"},
            {"mk48_adcap", FaseTubo::armato, 1, true, true, "ARMATO: PORTA APERTA"},
            {"mk67", FaseTubo::armato, 1, true, true, "MK 67 PRONTA ALLA POSA"},
            {"pmr1", FaseTubo::ricarica, 0.7, false, false, "PMR-1 IN RICARICA 70%"},
            {"set65", FaseTubo::armato, 1, true, true, "SET-65 ARMATO"},
        };
        int i = 0;
        for (const auto& c : casi) {
            const int col = i % 2, riga = i / 2;
            const Rettangolo r{8 + col * 316, 14 + riga * 116, 308, 96};
            SagomaInTubo s;
            if (*c.id) s.arma = sagoma_da_scheda(*cat.arma(c.id));
            s.stato = c.stato;
            s.ricarica = c.ricarica;
            s.allagato = c.allagato;
            s.porta_aperta = c.porta;
            s.tempo_s = 0.2;
            disegna_tubo(t, r, s, tav);
            font.scrivi(t, r.x, r.y - 10, c.testo, colori::GRIGIO, 1);
            ++i;
        }
    } else if (cosa == "pannello") {
        // tavolozza fissa: i riquadri da 82 x 21 del pannello Armi e una fila piu' grande
        const char* ids[] = {"mk48", "mk48_adcap", "harpoon", "tasm", "mk67", "mk60", "set65", "type53_65k", "pmr1", "shkval",
                             "adc_mk2", "mg84"};
        int i = 0;
        for (const char* id : ids) {
            const auto s = sagoma_da_scheda(*cat.arma(id));
            const int col = i % 6, riga = i / 6;
            const Rettangolo r{12 + col * 104, 20 + riga * 40, 82, 21};
            t.rettangolo_pieno(r.x - 1, r.y - 1, r.w + 2, r.h + 2, colori::GRIGIO_SCURO);
            SagomaInTubo q;
            q.arma = s;
            q.stato = i % 3 == 0 ? FaseTubo::armato : i % 3 == 1 ? FaseTubo::carico : FaseTubo::ricarica;
            q.ricarica = 0.5;
            q.allagato = q.stato == FaseTubo::armato;
            q.porta_aperta = q.allagato;
            disegna_tubo(t, r, q);
            font.scrivi(t, r.x, r.y + r.h + 3, id, colori::GRIGIO, 1);
            disegna_sagoma_arma(t, s, {12 + col * 104, 120 + riga * 30, 90, 14});
            ++i;
        }
        // in grande con la tavolozza fissa, di notte la vista non tocca questi indici
        int y = 190;
        for (const char* id : {"mk48_adcap", "mk60", "pmr1", "tasm", "type53_65k"}) {
            disegna_sagoma_arma(t, sagoma_da_scheda(*cat.arma(id)), {20, y, 600, 40});
            y += 56;
        }
    } else if (cosa == "esche") {
        const auto tav = TavolozzaSagome::ricca(pal);
        struct Caso { const char* id; FaseTubo stato; double ricarica; bool allagato, porta; const char* testo; };
        const Caso casi[] = {
            {"mg74", FaseTubo::carico, 1, false, false, "MG-74 KORUND CARICA"},
            {"mg74", FaseTubo::armato, 1, true, true, "MG-74 ARMATA: PORTA APERTA"},
            {"moss_mk70", FaseTubo::ricarica, 0.6, false, false, "MOSS MK 70 IN RICARICA 60%"},
            {"moss_mk70", FaseTubo::armato, 1, true, true, "MOSS MK 70 ARMATA (WHAT-IF)"},
        };
        int i = 0;
        for (const auto& c : casi) {
            const Rettangolo r{8 + (i % 2) * 316, 14 + (i / 2) * 110, 308, 90};
            SagomaInTubo s;
            s.arma = sagoma_da_scheda(*cat.arma(c.id));
            s.stato = c.stato;
            s.ricarica = c.ricarica;
            s.allagato = c.allagato;
            s.porta_aperta = c.porta;
            s.tempo_s = 0.2;
            disegna_tubo(t, r, s, tav);
            font.scrivi(t, r.x, r.y - 10, c.testo, colori::GRIGIO, 1);
            ++i;
        }
        // in grande alla stessa scala (100 px/m) e nei riquadri del pannello Armi (tavolozza fissa)
        int y = 240;
        for (const char* id : {"mg74", "moss_mk70", "mg84", "adc_mk2"}) {
            const auto s = sagoma_da_scheda(*cat.arma(id));
            disegna_sagoma_arma(t, s, {10, y, 440, 50}, 100.0, tav);
            font.scrivi(t, 460, y + 20, id, colori::GRIGIO, 1);
            SagomaInTubo q;
            q.arma = s;
            q.stato = FaseTubo::carico;
            disegna_tubo(t, {540, y + 14, 82, 21}, q);
            y += 58;
        }
    } else {
        std::fprintf(stderr, "modo sconosciuto: %s\n", cosa.c_str());
        return 2;
    }
    t.salva_bmp(uscita, pal);
    return 0;
}
