// Esempi numerici dei capitoli C1-C5 del manuale dell'Enhanced (volume 2), calcolati col motore: i numeri del testo
// vengono da qui, non da conti a mano. Non e' nella build: si compila a mano contro libnucleo.a.
//   g++ -std=c++20 -O2 -I engine/nucleo/include tools/manuale/esempi_acustica.cpp engine/build_acustica/libnucleo.a
//       -o engine/build_acustica/esempi_acustica && engine/build_acustica/esempi_acustica data/motore
#include <cstdio>
#include <string>

#include "nucleo/acustica.hpp"

using namespace m688;

namespace {

void stampa(const char* titolo, const Rilevazione& x) {
    std::printf("%s: SE %.1f dB a %.0f Hz | SL %.1f TL %.1f NL %.1f DI %.1f DT %.1f | percorso %s%s%s\n", titolo, x.se_db,
                x.banda_hz, x.sl_db, x.tl_db, x.nl_db, x.di_db, x.dt_db, nome_percorso(x.percorso), x.lofar ? " (LOFAR)" : "",
                x.baffles ? " (baffles)" : "");
}

double portata(Sorgente s, const Ricevitore& r, const Ambiente& amb) {
    double out = 0.0;
    for (double d = 200.0; d < 200000.0; d *= 1.02) {
        s.pos = {r.pos.x + d, r.pos.y};
        if (valuta(s, r, amb).se_db >= 0.0) out = d;
    }
    return out;
}

}  // namespace

int main(int argc, char** argv) {
    const std::string dati = argc > 1 ? argv[1] : "data/motore";
    const CatalogoAcustico cat = carica_acustica(dati + "/acustica.toml");
    const ClimaTeatro faroe = carica_clima(dati + "/teatri/faroe.suono.toml");
    Ambiente amb;
    amb.profilo = &faroe.profilo(faroe.mese_predefinito);
    amb.traffico = faroe.traffico;
    amb.ph = faroe.ph;
    amb.vento_m_s = 7.0;
    std::printf("Faroe, mese %d, vento 7 m/s, traffico %.2f\n", faroe.mese_predefinito, faroe.traffico);
    const auto& p = *amb.profilo;
    std::printf("profilo: strato %.0f m, asse %.0f m, prima CZ %.1f km\n", p.sld_m, p.asse_m, p.distanza_cz_m / 1000.0);

    const Sensore* tb16 = cat.sensore("tb16");
    const Sensore* sfera = cat.sensore("bqq5_sfera");
    const Ricevitore schiera{{0.0, 0.0}, 150.0, 0.0, 5.0, tb16};
    const Ricevitore prua{{0.0, 0.0}, 150.0, 0.0, 5.0, sfera};
    Sorgente v3;
    v3.firma = cat.firma("victor3");
    v3.velocita_kn = 5.0;
    v3.quota_m = 150.0;
    v3.rotta = 0.0;
    for (const double km : {3.0, 10.0, 20.0}) {
        v3.pos = {km * 1000.0, 0.0};
        char t[64];
        std::snprintf(t, sizeof t, "Victor III 5 kn a %.0f km, TB-16", km);
        stampa(t, valuta(v3, schiera, amb));
        std::snprintf(t, sizeof t, "Victor III 5 kn a %.0f km, sfera", km);
        stampa(t, valuta(v3, prua, amb));
    }
    std::printf("portate al traverso (TB-16 a 5 kn, 150 m):\n");
    for (const auto& [classe, kn] : {std::pair{"los_angeles", 5.0}, {"victor3", 5.0}, {"akula", 5.0}, {"akula", 15.0},
                                     {"alfa", 20.0}, {"charlie2", 5.0}, {"kilo", 3.0}}) {
        Sorgente s;
        s.firma = cat.firma(classe);
        s.velocita_kn = kn;
        s.quota_m = 150.0;
        std::printf("  %s a %.0f kn: %.1f km (sfera %.1f km)\n", classe, kn, portata(s, schiera, amb) / 1000.0,
                    portata(s, prua, amb) / 1000.0);
    }
    // soglie e direttivita'
    for (const char* id : {"tb16", "bqq5_sfera", "bqq5_attivo", "sovietico_3gen", "sovietico_2gen"}) {
        const Sensore* s = cat.sensore(id);
        if (!s) continue;
        std::printf("%s: DT di riferimento %.1f dB; DI", id, soglia(*s));
        for (const double f : s->bande_hz) std::printf(" %.0f Hz %.1f", f, indice_direttivita(*s, f));
        std::printf("; fascio a %.0f Hz %.1f gradi\n", s->bande_hz.front(), larghezza_fascio_deg(*s, s->bande_hz.front(), 90.0));
    }
    // probabilita' e fluttuazione
    for (const double se : {-4.0, 0.0, 4.0, 8.0})
        std::printf("P(scoperta) con SE %.0f dB e sigma 4: %.2f\n", se, probabilita_rilevazione(se, 4.0));
    // attivo: 688 che pinga un Victor III a 10 km
    const Sensore* att = cat.sensore("bqq5_attivo");
    const Ricevitore ping{{0.0, 0.0}, 150.0, 0.0, 5.0, att};
    v3.pos = {10000.0, 0.0};
    stampa("BQQ-5 attivo su Victor III a 10 km", valuta(v3, ping, amb));
    // rumore ambiente
    for (const double f : {100.0, 1000.0, 10000.0}) {
        const auto c = componenti_rumore_ambiente(f, amb);
        std::printf("rumore ambiente a %.0f Hz: %.1f dB\n", f, rumore_ambiente(f, amb));
        (void)c;
    }
    // C3: rumore ambiente per condizioni, a 100 Hz, 1 kHz e 10 kHz
    struct Cond { const char* nome; double vento, pioggia, traffico, ghiaccio; bool gamberi; };
    for (const Cond& c : {Cond{"calma, traffico 0,5", 0, 0, 0.5, 0, false}, Cond{"vento 5 m/s", 5, 0, 0.5, 0, false},
                          Cond{"vento 10 m/s", 10, 0, 0.5, 0, false}, Cond{"vento 15 m/s", 15, 0, 0.5, 0, false},
                          Cond{"traffico 1 (Gibilterra)", 5, 0, 1.0, 0, false}, Cond{"pioggia 10 mm/h, vento 5", 5, 10, 0.5, 0, false},
                          Cond{"gamberi, vento 5", 5, 0, 0.5, 0, true}, Cond{"margine del ghiaccio (50%)", 5, 0, 0.5, 0.5, false},
                          Cond{"pack (100%)", 5, 0, 0.5, 1.0, false}}) {
        Ambiente a;
        a.vento_m_s = c.vento, a.pioggia_mm_h = c.pioggia, a.traffico = c.traffico, a.ghiaccio = c.ghiaccio, a.gamberi = c.gamberi;
        std::printf("NL %s: %.1f / %.1f / %.1f dB\n", c.nome, rumore_ambiente(100, a), rumore_ambiente(1000, a), rumore_ambiente(10000, a));
    }
    // rumore proprio della TB-16 e della sfera con la velocita'
    for (const double kn : {3.0, 5.0, 8.0, 12.0, 16.0})
        std::printf("rumore proprio a 300 Hz, %.0f kn: TB-16 %.1f, sfera %.1f dB\n", kn, rumore_proprio(300, *tb16, kn, false),
                    rumore_proprio(300, *sfera, kn, false));
    // C2: perdita di trasmissione per distanza, nello strato e sotto, e nel Mediterraneo d'estate
    const ClimaTeatro otranto = carica_clima(dati + "/teatri/otranto.suono.toml");
    struct Caso { const char* nome; const ClimaTeatro* clima; int mese; double zs, zr, f; };
    for (const Caso& c : {Caso{"Faroe feb, 150/150 m, 1 kHz", &faroe, 2, 150, 150, 1000},
                          Caso{"Faroe feb, 150/400 m, 1 kHz", &faroe, 2, 150, 400, 1000},
                          Caso{"Faroe feb, 150/150 m, 250 Hz", &faroe, 2, 150, 150, 250},
                          Caso{"Otranto ago, 20/150 m, 1 kHz", &otranto, 8, 20, 150, 1000},
                          Caso{"Otranto ago, 100/100 m, 1 kHz", &otranto, 8, 100, 100, 1000}}) {
        Ambiente a = amb;
        a.profilo = &c.clima->profilo(c.mese);
        a.traffico = c.clima->traffico;
        a.ph = c.clima->ph;
        const auto& pr = *a.profilo;
        std::printf("%s (strato %.0f m, asse %.0f m, CZ %.1f km):", c.nome, pr.sld_m, pr.asse_m, pr.distanza_cz_m / 1000.0);
        for (const double km : {1.0, 5.0, 10.0, 20.0, 30.0, 50.0}) {
            const Perdita q = perdita_trasmissione(c.f, km * 1000.0, c.zs, c.zr, a);
            std::printf(" %.0f km %.1f %s;", km, q.tl_db, nome_percorso(q.percorso));
        }
        std::printf("\n");
    }
    // livello B: i raggi nello stesso Mediterraneo d'estate e alle Faroe
    for (const Caso& c : {Caso{"raggi Otranto ago, 100/100 m, 1 kHz", &otranto, 8, 100, 100, 1000},
                          Caso{"raggi Otranto ago, 20/150 m, 1 kHz", &otranto, 8, 20, 150, 1000},
                          Caso{"raggi Faroe feb, 150/400 m, 1 kHz", &faroe, 2, 150, 400, 1000}}) {
        Ambiente a = amb;
        a.profilo = &c.clima->profilo(c.mese);
        a.fondale_m = 3000.0;
        const CampoPerdita campo = calcola_campo(c.zs, a);
        std::printf("%s:", c.nome);
        for (const double km : {1.0, 5.0, 10.0, 20.0, 30.0, 50.0}) std::printf(" %.0f km %.1f;", km, campo.tl(c.f, km * 1000.0, c.zr).value_or(-1.0));
        std::printf("\n");
    }
    for (const double f : {100.0, 1000.0, 3500.0, 10000.0, 25000.0})
        std::printf("assorbimento a %.0f Hz (4 gradi, 35 psu): %.3f dB/km\n", f, assorbimento_db_km(f, 4.0, 35.0, 0.0, 8.0));
    return 0;
}
