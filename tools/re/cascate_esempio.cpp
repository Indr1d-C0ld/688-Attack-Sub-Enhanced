// Righe di cascata vere per le bozze del kit (passata estetica 33): un 688 con la schiera trainata sulla piattaforma
// Celtica (fondale 110 m, sabbia) e un mercantile in rotta opposta che gli passa a 3 km
// di CPA, al traverso. Scrive in tools/re/prove/cascate_esempio:
//   lofar_senza.f32, lofar_con.f32: 600 righe x 2000 bin (10-510 Hz, 0,25 Hz), una ogni 2 s, il fascio sul mercantile;
//   senza e con le frange d'interferenza (spettro_fascio con coerente);
//   btr.f32: 600 righe x 72 fasci (energia nella banda alta della schiera, con la grana);
//   meta.json: le dimensioni, le scale e la geometria.
// Valori (S + N) / N in dB, float32 little endian, riga per riga: quelli dei moduli, prima dei normalizzatori e del
// pavimento e tetto della stazione.
//
// Compilazione (dalla cartella engine, dopo ninja -C build_acustica):
//   c++ -O2 -std=c++20 -Inucleo/include -isystem terzi/tomlplusplus ../tools/re/cascate_esempio.cpp \
//       build_acustica/libnucleo.a -o build_acustica/cascate_esempio && build_acustica/cascate_esempio

#include <cmath>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

#include "nucleo/analisi_acustica.hpp"

using namespace m688;

int main() {
    const std::string dati = "../data/motore", uscita = "../tools/re/prove/cascate_esempio/";
    const CatalogoAcustico c = carica_acustica(dati + "/acustica.toml");
    const ClimaTeatro clima = carica_clima(dati + "/teatri/celtica.suono.toml");
    Ambiente amb;
    amb.profilo = &clima.profilo(9);
    amb.fondale_m = 110.0;
    amb.fondo = TipoFondo::sabbia;
    amb.vento_m_s = 4.0;
    amb.hs_m = 0.6;
    amb.traffico = 0.4;
    amb.ph = clima.ph;
    Ricevitore r;
    r.quota_m = 60.0;
    r.velocita_kn = 5.0;
    r.rotta = 0.0;
    r.sensore = c.sensore("tb16");
    Sorgente m;
    m.firma = c.firma("mercantile");
    m.nave = true;
    m.velocita_kn = 12.0;
    m.rotta = 180.0;
    constexpr int RIGHE = 600, FASCI = 72;
    constexpr double PASSO_S = 2.0, F0 = 10.0, F1 = 510.0, BIN = 0.25;
    std::ofstream senza(uscita + "lofar_senza.f32", std::ios::binary), con(uscita + "lofar_con.f32", std::ios::binary),
        btr(uscita + "btr.f32", std::ios::binary);
    Casuale caso(688);
    std::size_t bin = 0;
    for (int k = 0; k < RIGHE; ++k) {
        const double t = k * PASSO_S;
        r.pos = {0.0, 5.0 * NODO * t};
        m.pos = {3000.0, 5.0 * NODO * 0.5 * RIGHE * PASSO_S + 12.0 * NODO * (0.5 * RIGHE * PASSO_S - t)};   // CPA al traverso a meta'
        const double brg = rilevamento(r.pos, m.pos);
        const Spettro a = spettro_fascio({m}, r, amb, brg, F0, F1, BIN, false);
        const Spettro b = spettro_fascio({m}, r, amb, brg, F0, F1, BIN, true);
        const RigaCascata bt = cascata({m}, r, amb, FASCI, 0.0, &caso);
        senza.write(reinterpret_cast<const char*>(a.snr_db.data()), static_cast<std::streamsize>(a.snr_db.size() * sizeof(float)));
        con.write(reinterpret_cast<const char*>(b.snr_db.data()), static_cast<std::streamsize>(b.snr_db.size() * sizeof(float)));
        btr.write(reinterpret_cast<const char*>(bt.db.data()), static_cast<std::streamsize>(bt.db.size() * sizeof(float)));
        bin = a.snr_db.size();
    }
    std::ofstream meta(uscita + "meta.json");
    meta << "{\n  \"righe\": " << RIGHE << ", \"passo_s\": " << PASSO_S << ",\n"
         << "  \"lofar\": {\"bin\": " << bin << ", \"f0_hz\": " << F0 << ", \"df_hz\": " << BIN << ", \"file\": [\"lofar_senza.f32\", \"lofar_con.f32\"]},\n"
         << "  \"btr\": {\"fasci\": " << FASCI << ", \"rilevamento0\": 0, \"passo_gradi\": 5, \"file\": \"btr.f32\"},\n"
         << "  \"valori\": \"(S + N) / N in dB, float32 little endian, riga per riga dalla piu' vecchia\",\n"
         << "  \"scena\": \"688 con la TB-16 a 60 m, 5 kn per 000; mercantile a 12 kn per 180, CPA 3 km al traverso a meta'; Celtica, fondale 110 m di sabbia, settembre\"\n}\n";
    std::printf("%d righe, LOFAR %zu bin, BTR %d fasci, in %s\n", RIGHE, bin, FASCI, uscita.c_str());
}
