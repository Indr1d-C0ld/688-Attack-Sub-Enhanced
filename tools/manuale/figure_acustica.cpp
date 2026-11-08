// Dati delle figure dei capitoli C2, C3 e C8 del manuale dell'Enhanced (volume 2), calcolati col motore negli stessi
// casi di esempi_acustica.cpp. Scrive dei blocchi «# nome» con righe di numeri separati da spazi (fuori dalla griglia
// dei raggi «nan», che il disegno salta); li disegna tools/manuale/figure_acustica.py. Non e' nella build: si compila
// a mano contro libnucleo.a.
//   g++ -std=c++20 -O2 -I engine/nucleo/include tools/manuale/figure_acustica.cpp engine/build_kit/libnucleo.a \
//       -o engine/build_kit/figure_acustica && engine/build_kit/figure_acustica data/motore > tools/manuale/figure_acustica.dat
#include <cmath>
#include <cstdio>
#include <string>

#include "nucleo/acustica.hpp"
#include "nucleo/bordo_audio.hpp"

using namespace m688;

int main(int argc, char** argv) {
    const std::string dati = argc > 1 ? argv[1] : "data/motore";
    const ClimaTeatro faroe = carica_clima(dati + "/teatri/faroe.suono.toml");
    const ClimaTeatro otranto = carica_clima(dati + "/teatri/otranto.suono.toml");
    Ambiente amb;
    amb.traffico = faroe.traffico;
    amb.ph = faroe.ph;
    amb.vento_m_s = 7.0;

    // C2: perdita di trasmissione a 1 kHz per distanza (livello A e raggi), come in esempi_acustica.cpp
    {
        Ambiente a = amb;
        a.profilo = &faroe.profilo(2);
        Ambiente b = a;
        b.fondale_m = 3000.0;
        const CampoPerdita campo = calcola_campo(150.0, b);
        std::printf("# tl_faroe km A150_150 A150_400 B150_400\n");
        for (double km = 0.5; km <= 60.0 + 1e-9; km += 0.25)
            std::printf("%.2f %.2f %.2f %.2f\n", km, perdita_trasmissione(1000, km * 1000, 150, 150, a).tl_db,
                        perdita_trasmissione(1000, km * 1000, 150, 400, a).tl_db, campo.tl(1000, km * 1000, 400).value_or(NAN));
    }
    {
        Ambiente a = amb;
        a.profilo = &otranto.profilo(8);
        a.traffico = otranto.traffico;
        a.ph = otranto.ph;
        Ambiente b = amb;
        b.profilo = a.profilo;
        b.fondale_m = 3000.0;
        const CampoPerdita c100 = calcola_campo(100.0, b), c20 = calcola_campo(20.0, b);
        std::printf("# tl_otranto km A100_100 B100_100 B20_150\n");
        for (double km = 0.5; km <= 45.0 + 1e-9; km += 0.25)
            std::printf("%.2f %.2f %.2f %.2f\n", km, perdita_trasmissione(1000, km * 1000, 100, 100, a).tl_db,
                        c100.tl(1000, km * 1000, 100).value_or(NAN), c20.tl(1000, km * 1000, 150).value_or(NAN));
    }
    // C2: profilo della velocita' del suono alle Faroe in febbraio, coi suoi punti notevoli
    {
        const ProfiloSuono& p = faroe.profilo(2);
        std::printf("# profilo_faroe strato %.1f asse %.1f critica %.1f cz_km %.2f\n", p.sld_m, p.asse_m,
                    p.profondita_critica_m, p.distanza_cz_m / 1000.0);
        for (double z = 0.0; z <= 3000.0 + 1e-9; z += 10.0) std::printf("%.0f %.2f\n", z, p.c(z));
    }
    // C3: curve di Wenz del modello, traffico 0,5, vento 0, 5, 10 e 15 m/s
    {
        std::printf("# wenz hz v0 v5 v10 v15\n");
        for (double e = 0.0; e <= 5.0 + 1e-9; e += 0.05) {
            const double f = std::pow(10.0, e);
            std::printf("%.3f", f);
            for (const double v : {0.0, 5.0, 10.0, 15.0}) {
                Ambiente a;
                a.vento_m_s = v;
                a.traffico = 0.5;
                std::printf(" %.2f", rumore_ambiente(f, a));
            }
            std::printf("\n");
        }
    }
    // C8: lo scafo, dal livello fuori (dB re 1 uPa) a quello dentro (dB re 20 uPa), e la soglia d'udibilita'
    {
        std::printf("# scafo hz K_centrale K_sonar K_siluri soglia\n");
        for (double e = std::log10(20.0); e <= 4.0 + 1e-9; e += 0.025) {
            const double f = std::pow(10.0, e);
            std::printf("%.2f %.2f %.2f %.2f %.2f\n", f, trasferimento_scafo_db(f, StazioneBordo::centrale),
                        trasferimento_scafo_db(f, StazioneBordo::sonar), trasferimento_scafo_db(f, StazioneBordo::siluri),
                        soglia_udibilita_db(f));
        }
    }
    return 0;
}
