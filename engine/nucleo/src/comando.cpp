#include "nucleo/comando.hpp"

#include <algorithm>
#include <array>
#include <cmath>

namespace m688 {

namespace {
constexpr std::array<double, PostoDiComando::TELEGRAFO_POSIZIONI> POTENZA = {0.0, 0.0, 0.25, 0.5, 0.75, 1.0, 1.09375};
constexpr std::array<std::string_view, PostoDiComando::TELEGRAFO_POSIZIONI> NOMI = {"INDIETRO", "STOP", "1/3", "2/3",
                                                                                     "STD", "TUTTA", "MAX"};
}  // namespace

double PostoDiComando::potenza(int posizione) {
    return POTENZA[static_cast<std::size_t>(std::clamp(posizione, 0, TELEGRAFO_POSIZIONI - 1))];
}

std::string_view PostoDiComando::nome_telegrafo(int posizione) {
    return NOMI[static_cast<std::size_t>(std::clamp(posizione, 0, TELEGRAFO_POSIZIONI - 1))];
}

int PostoDiComando::posizione_da_velocita(double frazione) {
    int migliore = TELEGRAFO_STOP;
    for (int p = TELEGRAFO_STOP; p < TELEGRAFO_POSIZIONI; ++p)
        if (std::fabs(POTENZA[static_cast<std::size_t>(p)] - frazione) < std::fabs(POTENZA[static_cast<std::size_t>(migliore)] - frazione))
            migliore = p;
    return migliore;
}

void PostoDiComando::imposta_telegrafo(int posizione) { telegrafo_ = std::clamp(posizione, 0, TELEGRAFO_POSIZIONI - 1); }

void PostoDiComando::osserva(const Ordine& o) {
    switch (o.tipo) {
        case TipoOrdine::telegrafo: telegrafo_ = o.n == 1 ? TELEGRAFO_INDIETRO : posizione_da_velocita(o.a); break;
        case TipoOrdine::timone:
            timone_ = std::clamp(static_cast<int>(std::lround(o.a / 0.72)), -TIMONE_MAX, TIMONE_MAX);
            pilota_ = false;
            break;
        case TipoOrdine::quota: {   // barra: verso la quota massima (giu') o la superficie (su) a |scatto| / 4
            const int scatti = std::clamp(static_cast<int>(std::lround(o.b * 4.0)), 0, BARRA_MAX);
            barra_ = o.a >= QUOTA_MASSIMA_ORDINE - 1.0 ? scatti : (o.a == 0.0 && o.b <= 1.0) ? -scatti : 0;   // 0: rateo 0 o quota precisa
            break;
        }
        case TipoOrdine::quota_periscopica:
        case TipoOrdine::quota_150_piedi:
        case TipoOrdine::emersione_emergenza: barra_ = 0; break;
        case TipoOrdine::pilota_automatico: pilota_ = o.n != 0; break;
        default: break;
    }
}

Risposta PostoDiComando::azione(Azione a, const StatoProprio& s) {
    Risposta r;
    auto ordine = [&](TipoOrdine t, double x = 0.0, double y = 0.0, int n = 0) { r.ordine = Ordine{s.tempo, t, x, y, n}; };
    auto barra = [&](int scatto) {   // timoni orizzontali: verso la superficie o la quota massima; a zero tiene la quota
        barra_ = std::clamp(scatto, -BARRA_MAX, BARRA_MAX);
        if (barra_ == 0) ordine(TipoOrdine::quota, QUOTA_MASSIMA_ORDINE, 0.0);   // codice 17h: rateo 0, tiene la quota
        else ordine(TipoOrdine::quota, barra_ > 0 ? QUOTA_MASSIMA_ORDINE : 0.0, std::abs(barra_) / 4.0);
    };
    switch (a) {
        case Azione::timone_sinistra:
        case Azione::timone_dritta:
        case Azione::timone_centro: {
            const int nuovo = a == Azione::timone_centro ? 0 : std::clamp(timone_ + (a == Azione::timone_dritta ? 1 : -1), -TIMONE_MAX, TIMONE_MAX);
            if (nuovo == timone_ && a != Azione::timone_centro) return r;   // gia' a fondo corsa
            timone_ = nuovo;
            pilota_ = false;   // il timone disinserisce il pilota automatico
            ordine(TipoOrdine::timone, rateo_timone(timone_));
            return r;
        }
        case Azione::barra_su: barra(barra_ - 1); return r;
        case Azione::barra_giu: barra(barra_ + 1); return r;
        case Azione::barra_zero: barra(0); return r;
        case Azione::telegrafo_avanti:
        case Azione::telegrafo_indietro: {
            const int nuovo = std::clamp(telegrafo_ + (a == Azione::telegrafo_avanti ? 1 : -1), 0, TELEGRAFO_POSIZIONI - 1);
            if (nuovo == telegrafo_) return r;
            if (nuovo >= TELEGRAFO_STD && nuovo > telegrafo_ && s.quota < QUOTA_100_PIEDI) {
                r.rifiuto = MSG_TROPPO_IN_ALTO;
                return r;
            }
            telegrafo_ = nuovo;
            if (telegrafo_ == TELEGRAFO_INDIETRO) ordine(TipoOrdine::telegrafo, 0.0, 0.0, 1);   // arresto rapido
            else ordine(TipoOrdine::telegrafo, potenza(telegrafo_));
            return r;
        }
        case Azione::quota_periscopica: barra_ = 0; ordine(TipoOrdine::quota_periscopica); return r;
        case Azione::quota_150_piedi: barra_ = 0; ordine(TipoOrdine::quota_150_piedi); return r;
        case Azione::emersione_emergenza: barra_ = 0; ordine(TipoOrdine::emersione_emergenza); return r;
        case Azione::pilota_automatico:
            pilota_ = !pilota_;
            ordine(TipoOrdine::pilota_automatico, 0.0, 0.0, pilota_ ? 1 : 0);
            return r;
        case Azione::periscopio:
        case Azione::antenna: {
            const bool su = a == Azione::periscopio ? !s.periscopio : !s.antenna;
            if (su && s.quota > QUOTA_PERISCOPICA + 0.5) { r.rifiuto = MSG_SOLO_PERISCOPICA; return r; }
            ordine(a == Azione::periscopio ? TipoOrdine::periscopio : TipoOrdine::antenna, 0.0, 0.0, su ? 1 : 0);
            return r;
        }
        case Azione::impulso_sonar: ordine(TipoOrdine::impulso_sonar); return r;
        case Azione::posati:
        case Azione::immersione_emergenza: return r;   // solo Enhanced: li gestisce l'interfaccia con le stazioni
    }
    return r;
}

namespace {
// Campane della marina USA in nodi (avanti 1/3 = 5, 2/3 = 10, standard = 15, tutta = 20, massima = la massima);
// indietro come frazioni di indietro tutta (giri -0,5)
constexpr std::array<std::string_view, PostoEnhanced::CAMPANE> NOMI_CAMPANE = {
    "IND. TUTTA", "IND. 2/3", "IND. 1/3", "STOP", "AV. 1/3", "AV. 2/3", "STANDARD", "TUTTA", "MASSIMA"};
}  // namespace

PostoEnhanced::PostoEnhanced(std::function<double(double)> giri_per_nodi, double nodi_massimi)
    : giri_per_nodi_(std::move(giri_per_nodi)), nodi_max_(nodi_massimi) {}

std::string_view PostoEnhanced::nome_campana(int c) { return NOMI_CAMPANE[static_cast<std::size_t>(std::clamp(c, 0, CAMPANE - 1))]; }

double PostoEnhanced::nodi_campana(int c) const {
    switch (std::clamp(c, 0, CAMPANE - 1)) {
        case 4: return 5.0;
        case 5: return 10.0;
        case 6: return 15.0;
        case 7: return std::min(20.0, nodi_max_);
        case 8: return nodi_max_;
        default: return 0.0;
    }
}

void PostoEnhanced::osserva(const Ordine& o) {
    switch (o.tipo) {
        case TipoOrdine::barra: barra_ = static_cast<int>(std::lround(o.a)); break;
        case TipoOrdine::quota_con_angolo: quota_ = o.a; break;
        case TipoOrdine::emersione_emergenza: quota_ = 0.0; break;
        case TipoOrdine::giri: {
            double scarto = 1e9;
            for (int c = 0; c < CAMPANE; ++c) {
                const double g = c < STOP ? -0.5 * (STOP - c) / STOP : c == STOP ? 0.0 : giri_per_nodi_(nodi_campana(c));
                if (std::fabs(g - o.a) < scarto) { scarto = std::fabs(g - o.a); campana_ = c; }
            }
            break;
        }
        default: break;
    }
}

std::optional<Ordine> PostoEnhanced::azione(Azione a, const StatoProprio& s) {
    auto ordine = [&](TipoOrdine t, double x = 0.0, double y = 0.0) { return Ordine{s.tempo, t, x, y, 0}; };
    auto quota = [&](double q) {
        quota_ = std::max(0.0, q);
        return ordine(TipoOrdine::quota_con_angolo, quota_, ANGOLO_MAX);
    };
    switch (a) {
        case Azione::timone_sinistra:
        case Azione::timone_dritta:
        case Azione::timone_centro:
            barra_ = a == Azione::timone_centro ? 0
                                                : std::clamp(barra_ + (a == Azione::timone_dritta ? BARRA_PASSO : -BARRA_PASSO), -BARRA_MAX, BARRA_MAX);
            return ordine(TipoOrdine::barra, barra_);
        case Azione::telegrafo_avanti:
        case Azione::telegrafo_indietro: {
            const int nuova = std::clamp(campana_ + (a == Azione::telegrafo_avanti ? 1 : -1), 0, CAMPANE - 1);
            if (nuova == campana_) return std::nullopt;
            campana_ = nuova;
            const double giri = campana_ < STOP ? -0.5 * (STOP - campana_) / STOP
                                                : campana_ == STOP ? 0.0 : std::clamp(giri_per_nodi_(nodi_campana(campana_)), 0.0, 1.0);
            return ordine(TipoOrdine::giri, giri);
        }
        case Azione::barra_su: return quota(quota_ - QUOTA_PASSO_M);
        case Azione::barra_giu: return quota(quota_ + QUOTA_PASSO_M);
        case Azione::barra_zero: return quota(s.quota);   // tiene la quota attuale
        case Azione::quota_periscopica: return quota(QUOTA_PERISCOPICA_M);
        case Azione::quota_150_piedi: return quota(QUOTA_150_PIEDI_M);
        case Azione::emersione_emergenza: quota_ = 0.0; return ordine(TipoOrdine::emersione_emergenza);
        default: return std::nullopt;   // pilota automatico, periscopio, antenna, ping: come nel classico
    }
}

}  // namespace m688
