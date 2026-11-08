#include "nucleo/acustica_urto.hpp"

#include <algorithm>
#include <cmath>

namespace m688 {

namespace {

constexpr double COLPO_RIFERIMENTO_DB = 170.0;     // le voci del colpo: 1 MJ (16 §2.6, come livello_sorgente_db)
constexpr double STRIDIO_RIFERIMENTO_DB = 160.0;   // la voce dello stridio: 1 MJ in 1 s (STIMA)
constexpr double PEZZO_STRIDIO_S = 1.0;            // la voce dello stridio dura un secondo
constexpr int PEZZI_STRIDIO_MAX = 20;              // durata_strisciamento_s arriva a 20 s
constexpr double QUOTA_MINIMA_M = 0.5;             // un punto sopra il galleggiamento suona appena sotto

Transitorio t_(int& id, const char* tipo, const Urto& u, double t0, double guadagno_db, std::uint64_t seme) {
    Transitorio x;
    x.id = id++;
    x.tipo = tipo;
    x.pos = u.punto;
    x.quota_m = std::max(u.quota_punto_m, QUOTA_MINIMA_M);
    x.t0 = t0;
    x.origine = u.a.unita;
    x.guadagno_db = guadagno_db;
    x.seme = seme ^ (0x9e3779b97f4a7c15ull * static_cast<std::uint64_t>(x.id + 1));
    return x;
}

}  // namespace

double livello_stridio_db(double energia_tangente_mj, double durata_s) {
    const double potenza = std::max(energia_tangente_mj, 1e-6) / std::max(durata_s, 0.1);   // MJ al secondo
    return std::clamp(STRIDIO_RIFERIMENTO_DB + 10.0 * std::log10(potenza), 130.0, 180.0);
}

bool parte_squillante(ParteUrto p) {
    switch (p) {
        case ParteUrto::vela:
        case ParteUrto::alberi:
        case ParteUrto::timoni:
        case ParteUrto::eliche:
        case ParteUrto::schiera:
        case ParteUrto::sovrastrutture: return true;
        default: return false;
    }
}

std::vector<Transitorio> transitori_urto(const Urto& u, const EsitoUrto& e, int id_base, std::uint64_t seme) {
    std::vector<Transitorio> out;
    int id = id_base;
    const bool squilla = parte_squillante(e.a.parte) || parte_squillante(e.b.parte);
    out.push_back(t_(id, squilla ? "urto_lamiera" : "urto_colpo", u, u.tempo_s, e.livello_sorgente_db - COLPO_RIFERIMENTO_DB, seme));
    if (e.durata_strisciamento_s > 0.0 && e.energia_tangente_mj > 0.0) {
        const double l = livello_stridio_db(e.energia_tangente_mj, e.durata_strisciamento_s) - STRIDIO_RIFERIMENTO_DB;
        const int pezzi = std::clamp(static_cast<int>(std::ceil(e.durata_strisciamento_s / PEZZO_STRIDIO_S - 1e-9)), 1, PEZZI_STRIDIO_MAX);
        for (int k = 0; k < pezzi; ++k) out.push_back(t_(id, "urto_stridio", u, u.tempo_s + k * PEZZO_STRIDIO_S, l, seme));
    }
    // l'elica che colpisce (di chi ha l'elica danneggiata, o le eliche come parte): i colpi delle pale, 20 dB sotto
    if (e.a.parte == ParteUrto::eliche || e.b.parte == ParteUrto::eliche || e.a.eliche_danneggiate || e.b.eliche_danneggiate)
        out.push_back(t_(id, "urto_elica", u, u.tempo_s, e.livello_sorgente_db - COLPO_RIFERIMENTO_DB, seme));
    return out;
}

}  // namespace m688
