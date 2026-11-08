// Anteprima dell'albo d'onore (app/albo_onore): un albo di prova (missioni, rapide col codice, carriere con i caduti
// delle due marine) e le schede dei comandanti storici di data/motore.
// Uso: anteprima_albo_onore <dati> <uscita> [vuoto]   scrive <uscita>_1.bmp, _2.bmp, _3.bmp (le tre schede)
#include <cstdio>
#include <filesystem>
#include <string>

#include "albo_onore.hpp"
#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "nucleo/albo_onore.hpp"
#include "nucleo/carriera.hpp"

using namespace m688;

int main(int argc, char** argv) {
    if (argc < 3) {
        std::fprintf(stderr, "uso: anteprima_albo_onore <dati> <uscita> [vuoto]\n");
        return 2;
    }
    const std::string dati = argv[1], uscita = argv[2];
    const bool vuoto = argc > 3 && std::string(argv[3]) == "vuoto";
    const auto regole = carica_carriera(dati + "/carriera.toml");
    const auto font = Font::carica(dati + "/font/originale.fnt");
    const auto file = std::filesystem::temp_directory_path() / "m688_anteprima_albo_onore.toml";
    AlboOnore a;
    if (!vuoto) {
        const char* nomi[] = {"Mark Reynolds", "Aleksej Voronin", "James T. Harlan", "Sergej Kuznecov", "Paul Whitaker"};
        const char* missioni[] = {"SCENE4A", "SCENE7B", "Rapida", "SCENE2C", "Libreria: LIB-I02", "SCENE5A"};
        for (int i = 0; i < 20; ++i) {
            VoceOnoreMissione v;
            v.punteggio = 1850 - i * 63;
            v.nome = nomi[i % 5];
            v.marina = i % 5 == 1 || i % 5 == 3 ? "vmf" : "usn";
            v.grado = v.marina == "vmf" ? (i % 2 ? "kap2" : "kap3") : (i % 2 ? "O-5" : "O-4");
            v.missione = missioni[i % 6];
            v.origine = i % 6 == 2 ? "rapida" : i % 6 == 4 ? "libreria" : "originale";
            if (v.origine == "rapida") v.codice = i % 4 ? "7F3A-91C2-04BE5" : "0D11-6A5F-E2C97";   // 13 caratteri
            else if (i % 3 != 2) v.codice = i % 2 ? "K7Q2M9-X4T1B8-Z3N6P0" : "W8W8W8-MMMMMM-WWWWWW";   // partita: 18
            v.data = "1989-" + std::to_string(1 + i % 12);
            v.data_reale = "2026-10-" + std::to_string(10 + i % 17);
            a.missioni.push_back(v);
        }
        const char* stati[] = {"pensione", "morto", "congedo", "rimosso", "morto", "trasferito", "pensione", "congedo", "pensione", "morto"};
        for (int i = 0; i < 10; ++i) {
            VoceOnoreCarriera c;
            c.punti = 24000 - i * 1700;
            c.missioni = 31 - i * 2;
            c.nome = nomi[(i + 2) % 5];
            c.marina = (i + 2) % 5 == 1 || (i + 2) % 5 == 3 ? "vmf" : "usn";
            c.grado = c.marina == "vmf" ? "kap1" : "O-6";
            c.decorazione = c.marina == "vmf" ? "Ordine della Bandiera Rossa" : (i % 2 ? "Navy Cross" : "Legion of Merit");
            c.stato = stati[i];
            c.data = "199" + std::to_string(i % 6) + "-0" + std::to_string(1 + i % 9);
            a.carriere.push_back(c);
        }
    }
    salva_albo_onore(a, file);
    SchermataAlboOnore s(regole, dati, file);
    Palette p = Palette::carica(dati + "/palette/giorno.pal");
    s.palette(p);
    for (int k = 0; k < 3; ++k) {
        s.scheda(k);
        Tela t;
        s.disegna(t, font);
        t.salva_bmp(uscita + "_" + std::to_string(k + 1) + ".bmp", p);
    }
    std::filesystem::remove(file);
    return 0;
}
