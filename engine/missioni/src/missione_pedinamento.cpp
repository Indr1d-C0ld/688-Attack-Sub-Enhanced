// Il pedinamento ravvicinato dell'IA (S24 «Quota zero», il Tautog e il K-108 del 1970; principale, 31/10/2026). Un
// sottomarino dell'IA, di qualunque parte (simmetria), si tiene nel cono sordo di un altro, a poche centinaia di metri,
// e ne segue la rotta, la velocita' e la quota con il ritardo con cui le vede:
// - il cono sordo dai sensori del pedinato (Sensore::baffles_gradi dei sonar di scafo; la schiera trainata filata lo
//   restringe al suo endfire, contato simmetrico: STIMA), il piu' stretto fra quelli in ascolto;
// - quello che il pedinatore sa del pedinato: la posizione dal suo contatto (con l'errore della stima), la rotta, la
//   velocita' e la quota viste. Un cambio oltre la soglia arriva dopo un ritardo (RITARDO_MIN_S-RITARDO_MAX_S: la deriva
//   del rilevamento, il segnale che cambia; STIMA) e la quota vista ha l'errore della stima (SIGMA_QUOTA_M);
// - il punto voluto: dietro il pedinato lungo la sua rotta vista, alla distanza chiesta, e la quota vista piu' lo scarto;
//   la velocita' quella vista, corretta sull'errore di distanza;
// - l'anticollisione dell'IA (missione_difesa.cpp) non guarda la coppia: la evita il pedinatore stesso, sulla geometria
//   vista e col suo ritardo, cosi' un'accostata o un cambio di quota brusco del pedinato puo' portare all'urto (il Tautog);
// - senza contatto per PERSO_S il pedinamento finisce. Il registro ha la reazione «pedina_vicino» e la sua fine.
// Solo nell'Enhanced (con le forze); il classico non cambia.

#include <algorithm>
#include <cmath>

#include "missioni/missione.hpp"

namespace m688 {

namespace {

constexpr double SOGLIA_ROTTA_GRADI = 10.0, SOGLIA_QUOTA_M = 10.0, SOGLIA_VELOCITA_KN = 2.0;   // un cambio che si nota
constexpr double RITARDO_MIN_S = 20.0, RITARDO_MAX_S = 60.0;   // prima che il pedinatore se ne accorga (STIMA)
constexpr double SIGMA_QUOTA_M = 8.0;                         // la quota del pedinato stimata dal passivo (STIMA)
constexpr double CPA_VISTO_M = 120.0;                         // sulla geometria vista: sotto, si scansa (STIMA)
constexpr double PERSO_S = 300.0;
constexpr double CONO_SENZA_SENSORI_GRADI = 60.0;

}  // namespace

double Missione::cono_sordo_gradi(int id) const {
    const auto ib = unita_.find(id);
    if (ib == unita_.end()) return 0.0;
    double cono = -1.0;
    for (const Sensore* s : sensori_acustici(ib->second, TipoSensori::ascolto)) {
        const double c = s->antenna == Antenna::lineare ? s->endfire_gradi : s->baffles_gradi;
        cono = cono < 0.0 ? c : std::min(cono, c);
    }
    return cono < 0.0 ? CONO_SENZA_SENSORI_GRADI : cono;
}

bool Missione::pedina_vicino(int u, int b, double distanza_m, double sopra_m) {
    const auto* o = trova(u);
    if (!o || o->giocatore || o->categoria != "sottomarino") return false;
    if (b < 0) {
        if (pedine_vicine_.erase(u)) reazioni_ia_.push_back({tempo_, u, -1, "sottomarino", "fine_pedina_vicino", "script"});
        return true;
    }
    const auto* t = trova(b);
    if (!t || t->categoria != "sottomarino" || b == u) return false;
    PedinaVicino p;
    p.bersaglio = b;
    p.distanza_m = std::max(100.0, distanza_m);
    p.sopra_m = sopra_m;
    p.rotta_vista = t->rotta;
    p.quota_vista = t->quota;
    p.velocita_vista = t->velocita;
    p.pos_vista = t->pos;
    p.ultimo_contatto = tempo_;
    pedine_vicine_[u] = p;
    reazioni_ia_.push_back({tempo_, u, b, "sottomarino", "pedina_vicino", "script"});
    return true;
}

bool Missione::in_pedinamento_vicino(int u, int b) const {
    const auto it = pedine_vicine_.find(u);
    return it != pedine_vicine_.end() && (b < 0 || it->second.bersaglio == b);
}

void Missione::passo_pedina_vicino(double dt) {
    (void)dt;
    for (auto it = pedine_vicine_.begin(); it != pedine_vicine_.end();) {
        auto* o = trova(it->first);
        const auto* t = trova(it->second.bersaglio);
        PedinaVicino& p = it->second;
        if (!o || !t || o->distrutta || o->persa || t->distrutta || t->persa) { it = pedine_vicine_.erase(it); continue; }
        // il contatto: senza, la posizione resta l'ultima stimata e dopo PERSO_S si smette
        const auto c = o->contatti.find(t->id);
        const bool tiene = c != o->contatti.end() && tempo_ - c->second.ultima_scoperta <= 30.0;
        if (tiene) {
            p.ultimo_contatto = tempo_;
            p.pos_vista = c->second.posizione_nota ? c->second.pos_stimata : t->pos;
        } else if (tempo_ - p.ultimo_contatto > PERSO_S) {
            reazioni_ia_.push_back({tempo_, it->first, t->id, "sottomarino", "fine_pedina_vicino", "perso"});
            it = pedine_vicine_.erase(it);
            continue;
        }
        // i cambi del pedinato arrivano col ritardo
        const bool cambia = std::fabs(normalizza_180(t->rotta - p.rotta_vista)) > SOGLIA_ROTTA_GRADI ||
                            std::fabs(t->quota - p.quota_vista) > SOGLIA_QUOTA_M || std::fabs(t->velocita - p.velocita_vista) > SOGLIA_VELOCITA_KN;
        if (tiene && cambia) {
            if (p.cambio_da < 0.0) {
                p.cambio_da = tempo_;
                p.ritardo_s = caso_reazioni_.uniforme(RITARDO_MIN_S, RITARDO_MAX_S);
            } else if (tempo_ - p.cambio_da >= p.ritardo_s) {
                p.rotta_vista = t->rotta;
                p.velocita_vista = t->velocita;
                const double u1 = std::max(caso_reazioni_.uniforme(), 1e-12), u2 = caso_reazioni_.uniforme();
                const double gauss = std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * 3.14159265358979323846 * u2);
                p.quota_vista = std::max(0.0, t->quota + SIGMA_QUOTA_M * gauss);
                p.cambio_da = -1.0;
            }
        } else if (!cambia) {
            p.cambio_da = -1.0;
        }
        // il punto voluto: nel cono sordo, dietro lungo la rotta vista
        const double r = radianti(p.rotta_vista);
        const Vec2 punto{p.pos_vista.x - p.distanza_m * std::sin(r), p.pos_vista.y - p.distanza_m * std::cos(r)};
        const double errore = distanza(o->pos, p.pos_vista) - p.distanza_m;   // > 0: troppo indietro
        double vel = std::clamp(p.velocita_vista + 0.01 * errore, 0.0, o->avanti_tutta);
        double quota = std::clamp(p.quota_vista - p.sopra_m, 20.0, o->quota_max - 20.0);   // sopra_m > 0: piu' in alto
        // la sua anticollisione, sulla geometria vista (rotta e velocita' viste del pedinato) e quindi col ritardo
        const double rv = radianti(o->rotta), vv = metri_al_secondo(o->velocita), vt = metri_al_secondo(p.velocita_vista);
        const Vec2 dp{p.pos_vista.x - o->pos.x, p.pos_vista.y - o->pos.y};
        const Vec2 dv{vt * std::sin(r) - vv * std::sin(rv), vt * std::cos(r) - vv * std::cos(rv)};
        const double dv2 = dv.x * dv.x + dv.y * dv.y;
        const double tc = dv2 > 1e-6 ? -(dp.x * dv.x + dp.y * dv.y) / dv2 : -1.0;
        Vec2 verso = punto;
        if (tc > 0.0 && tc < 120.0 && std::hypot(dp.x + dv.x * tc, dp.y + dv.y * tc) < CPA_VISTO_M && std::fabs(o->quota - p.quota_vista) < 30.0) {
            const double via = radianti(normalizza_360(rilevamento(p.pos_vista, o->pos)));
            verso = {o->pos.x + 2000.0 * std::sin(via), o->pos.y + 2000.0 * std::cos(via)};
            vel = std::max(vel, 5.0);
            quota = std::clamp(p.quota_vista + (p.sopra_m >= 0.0 ? -40.0 : 40.0), 20.0, o->quota_max - 20.0);   // via dalla sua parte
        }
        manovre_difesa_[it->first] = {tempo_ + 2.0, verso, vel, manovre_difesa_.count(it->first) ? manovre_difesa_.at(it->first).velocita_prima : o->velocita_ordinata};
        o->quota_ordinata = quota;
        o->rateo_quota = std::max(o->rateo_quota, o->rateo_quota_modello);
        ++it;
    }
}

}  // namespace m688
