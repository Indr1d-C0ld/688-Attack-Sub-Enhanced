#pragma once
// La copertura del ghiaccio per la carta dello spettatore (il vetrino di Dio, D4; docs/28 §3.3): una griglia grossolana del
// meteo sul teatro, nei metadati della traccia, perche' il replay da file non ha la partita. Da questa e dai record X si rifa'
// il CampoGhiaccio (app/segni_ambiente.hpp, campo_spettatore): il pack col suo bordo, la deriva cambiata e le polynye.
// Solo nel vetrino, e solo se il teatro ha ghiaccio: le altre tracce restano byte per byte.
// Il testo: "x0;y0;passo;nx;ny;" e due cifre esadecimali per cella (0-255), riga per riga verso nord. Un passo fine, perche' il
// clima cambia di poco fra due celle: a passi del 5% il bordo a 0,5 si attacca ai centri delle celle e viene a gradini.
// STIMA: la copertura del momento vale per tutta la partita; il meteo la cambia solo fra i mesi.
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <optional>
#include <string>
#include <vector>

#include "missioni/missione.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/meteo.hpp"

namespace m688 {

inline constexpr const char* METADATO_GRIGLIA_GHIACCIO = "griglia_ghiaccio";
// il mese dell'acqua, per il profilo del suono nella scheda dello spettatore (app/segni_ambiente.hpp): anche questo solo nel vetrino
inline constexpr const char* METADATO_MESE_ACUSTICA = "mese_acustica";

struct GrigliaGhiaccio {
    double x0 = 0.0, y0 = 0.0, passo_m = 10000.0;   // il centro della prima cella, il lato
    int nx = 0, ny = 0;
    std::vector<std::uint8_t> celle;                // da 0 a 255

    // bilineare fra i centri delle celle; fuori dalla griglia il bordo
    double in(Vec2 p) const {
        const double fx = std::clamp((p.x - x0) / passo_m, 0.0, nx - 1.0), fy = std::clamp((p.y - y0) / passo_m, 0.0, ny - 1.0);
        const int i = std::min(static_cast<int>(fx), nx - 1), j = std::min(static_cast<int>(fy), ny - 1);
        const int i1 = std::min(i + 1, nx - 1), j1 = std::min(j + 1, ny - 1);
        const double dx = fx - i, dy = fy - j;
        const auto c = [&](int a, int b) { return celle[static_cast<std::size_t>(b) * nx + a] / 255.0; };
        return (1.0 - dy) * ((1.0 - dx) * c(i, j) + dx * c(i1, j)) + dy * ((1.0 - dx) * c(i, j1) + dx * c(i1, j1));
    }
};

inline std::string codifica_griglia(const GrigliaGhiaccio& g) {
    char b[128];
    std::snprintf(b, sizeof b, "%.17g;%.17g;%.17g;%d;%d;", g.x0, g.y0, g.passo_m, g.nx, g.ny);
    std::string s = b;
    s.reserve(s.size() + 2 * g.celle.size());
    for (const std::uint8_t c : g.celle) {
        s += "0123456789abcdef"[c >> 4];
        s += "0123456789abcdef"[c & 15];
    }
    return s;
}

// nullopt se il testo non torna (una traccia rovinata non deve fermare il replay: niente pack)
inline std::optional<GrigliaGhiaccio> leggi_griglia(const std::string& s) {
    GrigliaGhiaccio g;
    int usati = 0;
    if (std::sscanf(s.c_str(), "%lf;%lf;%lf;%d;%d;%n", &g.x0, &g.y0, &g.passo_m, &g.nx, &g.ny, &usati) != 5 || usati <= 0) return std::nullopt;
    if (g.nx <= 0 || g.ny <= 0 || g.passo_m <= 0.0 || s.size() - usati != 2 * static_cast<std::size_t>(g.nx) * g.ny) return std::nullopt;
    const auto cifra = [](char c) { return c >= '0' && c <= '9' ? c - '0' : c >= 'a' && c <= 'f' ? c - 'a' + 10 : -1; };
    g.celle.reserve((s.size() - usati) / 2);
    for (std::size_t k = usati; k < s.size(); k += 2) {
        const int a = cifra(s[k]), b = cifra(s[k + 1]);
        if (a < 0 || b < 0) return std::nullopt;
        g.celle.push_back(static_cast<std::uint8_t>(a * 16 + b));
    }
    return g;
}

// La griglia della missione, presa prima del primo passo. Vuota fuori dal vetrino, senza meteo o fondale, e dove il teatro non ha ghiaccio
inline std::string griglia_ghiaccio_missione(const Missione& m) {
    constexpr double PASSO_M = 10000.0;   // STIMA: il bordo del pack sulla carta, non il campo; il meteo e' interpolato fra i suoi punti
    constexpr int LATO_MAX = 200;
    const Meteo* meteo = m.meteo();
    const Fondale* fondo = m.fondale();
    if (!m.sandbox() || !meteo || !fondo) return {};
    const double lx = fondo->larghezza() * fondo->cella_m(), ly = fondo->altezza() * fondo->cella_m();
    GrigliaGhiaccio g;
    g.passo_m = std::max({PASSO_M, lx / LATO_MAX, ly / LATO_MAX});
    g.nx = std::max(1, static_cast<int>(std::ceil(lx / g.passo_m)));
    g.ny = std::max(1, static_cast<int>(std::ceil(ly / g.passo_m)));
    g.x0 = -lx / 2.0 + g.passo_m / 2.0;   // il fondale e' centrato sull'origine delle missioni (Fondale::centro_cella)
    g.y0 = -ly / 2.0 + g.passo_m / 2.0;
    g.celle.reserve(static_cast<std::size_t>(g.nx) * g.ny);
    bool ghiaccio = false;
    for (int j = 0; j < g.ny; ++j)
        for (int i = 0; i < g.nx; ++i) {
            const double c = meteo->in({g.x0 + i * g.passo_m, g.y0 + j * g.passo_m}).ghiaccio;
            const auto k = static_cast<std::uint8_t>(std::lround(std::clamp(c, 0.0, 1.0) * 255.0));
            ghiaccio = ghiaccio || k > 0;
            g.celle.push_back(k);
        }
    return ghiaccio ? codifica_griglia(g) : std::string();
}

}  // namespace m688
