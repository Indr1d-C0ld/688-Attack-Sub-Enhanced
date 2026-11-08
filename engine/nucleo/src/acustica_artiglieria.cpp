#include "nucleo/acustica_artiglieria.hpp"

#include <algorithm>
#include <cmath>

namespace m688 {

namespace {

constexpr double CARICA_RIFERIMENTO_KG = 3.5;    // la voce dello scoppio: un 5" o un 130 mm
constexpr double CARICA_RAFFICA_KG = 0.05;       // la voce della raffica: un 30 mm (OF-84, 48,5 g)
constexpr double CALIBRO_RIFERIMENTO_MM = 127.0;
constexpr double PEZZO_RAFFICA_S = 0.5;          // la voce della raffica dura mezzo secondo
constexpr double QUOTA_SCOPPIO_M = 0.5, QUOTA_PARTENZA_M = 3.0;

// lo scoppio a specchio scala come W^0,12 (picco di Cole diviso theta): 2,4 log W, come la tabella di 22 §7.1
double guadagno_carica_db(double kg, double riferimento) { return 2.4 * std::log10(std::max(kg, 1e-3) / riferimento); }
// partenza e tonfo con la massa del proietto, ~calibro^3 (STIMA)
double guadagno_calibro_db(double mm) { return 30.0 * std::log10(std::max(mm, 5.0) / CALIBRO_RIFERIMENTO_MM); }

Transitorio t_(int& id, const char* tipo, Vec2 pos, double quota_m, double t0, double guadagno_db, std::uint64_t seme, int origine) {
    Transitorio x;
    x.id = id++;
    x.tipo = tipo;
    x.pos = pos;
    x.quota_m = quota_m;
    x.t0 = t0;
    x.origine = origine;
    x.guadagno_db = guadagno_db;
    x.seme = seme ^ (0x9e3779b97f4a7c15ull * static_cast<std::uint64_t>(x.id + 1));
    return x;
}

}  // namespace

const char* nome_evento_artiglieria(TipoEventoArtiglieria t) {
    switch (t) {
        case TipoEventoArtiglieria::partenza: return "partenza";
        case TipoEventoArtiglieria::scoppio_superficie: return "scoppio_superficie";
        case TipoEventoArtiglieria::scoppio_aria: return "scoppio_aria";
        case TipoEventoArtiglieria::tonfo: return "tonfo";
        case TipoEventoArtiglieria::rimbalzo: return "rimbalzo";
        case TipoEventoArtiglieria::raffica: return "raffica";
        case TipoEventoArtiglieria::colpo_testa: return "colpo_testa";
        case TipoEventoArtiglieria::passaggio: return "passaggio";
        case TipoEventoArtiglieria::illuminante: return "illuminante";
    }
    return "?";
}

double picco_scoppio_db(double tnt_kg, double distanza_m, double quota_ricevitore_m, double quota_scoppio_m) {
    const double w3 = std::cbrt(std::max(tnt_kg, 1e-4)), r = std::max(distanza_m, 1.0);
    const double p = 52.4e6 * std::pow(w3 / r, 1.13);                        // Pa
    const double theta = 0.084e-3 * w3 * std::pow(w3 / r, -0.23);            // s
    const double dt = 2.0 * std::max(quota_scoppio_m, 0.01) * std::max(quota_ricevitore_m, 0.01) / (r * 1500.0);
    return 20.0 * std::log10(std::max(p * (1.0 - std::exp(-dt / theta)), 1e-12) / 1e-6);
}

std::vector<Transitorio> transitori_artiglieria(const EventoArtiglieria& e, int id_base, std::uint64_t seme) {
    std::vector<Transitorio> out;
    int id = id_base;
    const int o = e.tiratore;
    switch (e.tipo) {
        case TipoEventoArtiglieria::partenza:
            out.push_back(t_(id, "art_partenza", e.pos, QUOTA_PARTENZA_M, e.tempo_s, guadagno_calibro_db(e.calibro_mm), seme, o));
            break;
        case TipoEventoArtiglieria::scoppio_superficie:
            out.push_back(t_(id, "art_scoppio_superficie", e.pos, QUOTA_SCOPPIO_M, e.tempo_s,
                             guadagno_carica_db(e.carica_kg, CARICA_RIFERIMENTO_KG), seme, o));
            break;
        case TipoEventoArtiglieria::scoppio_aria:
        case TipoEventoArtiglieria::colpo_testa:   // il proietto scoppia sull'albero, sopra l'acqua
            out.push_back(t_(id, "art_scoppio_aria", e.pos, QUOTA_SCOPPIO_M, e.tempo_s,
                             guadagno_carica_db(e.carica_kg, CARICA_RIFERIMENTO_KG), seme, o));
            break;
        case TipoEventoArtiglieria::tonfo:
            out.push_back(t_(id, "art_tonfo", e.pos, QUOTA_SCOPPIO_M, e.tempo_s, guadagno_calibro_db(e.calibro_mm), seme, o));
            break;
        case TipoEventoArtiglieria::rimbalzo:
            out.push_back(t_(id, "art_tonfo", e.pos, QUOTA_SCOPPIO_M, e.tempo_s, guadagno_calibro_db(e.calibro_mm) - 3.0, seme, o));
            break;
        case TipoEventoArtiglieria::raffica: {
            const double durata = std::max(1, e.colpi) * std::max(e.intervallo_s, 0.0);
            const int pezzi = std::clamp(static_cast<int>(std::ceil(durata / PEZZO_RAFFICA_S)), 1, 7);
            for (int k = 0; k < pezzi; ++k)
                out.push_back(t_(id, "art_raffica", e.pos, QUOTA_SCOPPIO_M, e.tempo_s + k * PEZZO_RAFFICA_S,
                                 guadagno_carica_db(e.carica_kg > 0.0 ? e.carica_kg : CARICA_RAFFICA_KG, CARICA_RAFFICA_KG), seme, o));
            break;
        }
        case TipoEventoArtiglieria::passaggio:
        case TipoEventoArtiglieria::illuminante: break;   // in aria: solo a bordo, in plancia
    }
    return out;
}

}  // namespace m688
