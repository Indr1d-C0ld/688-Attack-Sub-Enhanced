// Posarsi sul fondo nell'Enhanced: i rischi (roadmap 30; docs/11 §6, C6c). Il tipo di fondo e' quello del teatro e della
// profondita' (ClimaTeatro::fondo: piattaforma, scarpata, profondo); i numeri sono STIMA DI GIOCO, senza fonti aperte.
// - Fango: la presa cresce col tempo posato (piena in 10 minuti); a lasciare il fondo il battello resta trattenuto per
//   presa x 180 s da fermo (la quota resta sul fondo: navigazione), poi si libera; le macchine avanti accorciano
//   l'attesa (a 4 nodi la meta').
// - Roccia: strisciare oltre mezzo nodo, posato, danneggia ogni minuto con P = 0,05 + 0,05 v il timone o la cupola del
//   sonar di prua (guasti_ che applica DanniSottomarini) e lo scafo (integrita').
// - Prese a mare: posato sul fango piu' di mezz'ora, ogni minuto P = 0,1 che aspirino il sedimento: il raffreddamento
//   in avaria, la propulsione al 50% (perche' i nucleari evitano di posarsi).
// Per il giocatore e per l'IA, con la stessa regola. Il classico non lo usa (serve il clima del teatro).

#include <algorithm>
#include <cmath>

#include "missioni/missione.hpp"
#include "nucleo/battello.hpp"

namespace m688 {

namespace {

constexpr double PRESA_PIENA_S = 600.0, PRESA_DOPO_S = 60.0;   // la presa del fango comincia dopo un minuto posato
constexpr double TRATTENUTO_MAX_S = 180.0, SPINTA_KN = 4.0;   // da fermo 180 s; a 4 nodi la meta'
constexpr double STRISCIA_KN = 0.5;
constexpr double PRESE_DOPO_S = 1800.0, P_PRESE_MINUTO = 0.1, PERDITA_PRESE = 0.5;
constexpr double PERDITA_ROCCE = 0.3, DANNO_ROCCE = 0.02;      // del sistema; dello scafo, sull'integrita' massima
constexpr int MSG_FANGO = 476, MSG_ROCCE = 477, MSG_PRESE = 478;

}  // namespace

std::optional<Missione::StatoFondoUnita> Missione::stato_fondo(int id) const {
    const auto it = fondi_.find(id);
    if (it == fondi_.end()) return std::nullopt;
    return it->second.s;
}

bool Missione::trattieni_sul_fondo(UnitaClassica& u) {
    const auto it = fondi_.find(u.id);
    if (it == fondi_.end() || it->second.s.tipo != TipoFondo::fango || it->second.s.presa <= 0.0) return false;
    StatoFondoUnita& s = it->second.s;
    if (s.trattenuto_s <= 0.0) {   // il primo tentativo di lasciare il fondo: comincia la presa
        s.trattenuto_s = s.presa * TRATTENUTO_MAX_S;
        if (u.giocatore) messaggio_di_bordo(u.id, MSG_FANGO);
    }
    return true;
}

void Missione::passo_fondo(double dt) {
    if (!caso_fondo_) caso_fondo_.emplace(seme_ ^ 0xF0D011ull);
    Casuale& caso = *caso_fondo_;
    for (auto& [id, u] : unita_) {
        if (u.distrutta || u.persa || u.categoria != "sottomarino" || (u.flag & 0x20)) continue;
        if (!u.sul_fondo) {
            if (const auto it = fondi_.find(id); it != fondi_.end()) {   // staccato: la presa si azzera
                it->second.s.posato_s = it->second.s.presa = it->second.s.trattenuto_s = 0.0;
                it->second.avvisato_rocce = false;
            }
            continue;
        }
        FondoUnita& f = fondi_[id];
        StatoFondoUnita& s = f.s;
        s.tipo = clima_->fondo(profondita(u.pos));
        s.posato_s += dt;
        if (s.trattenuto_s > 0.0) {   // trattenuto: il conto alla rovescia, poi libero (la presa riparte da capo)
            s.trattenuto_s = std::max(0.0, s.trattenuto_s - dt * (1.0 + std::fabs(u.velocita) / SPINTA_KN));
            if (s.trattenuto_s <= 0.0) s.presa = s.posato_s = 0.0;
        } else if (s.tipo == TipoFondo::fango) {
            s.presa = std::clamp((s.posato_s - PRESA_DOPO_S) / PRESA_PIENA_S, 0.0, 1.0);
        }
        f.minuto_s += dt;
        if (f.minuto_s < 60.0) continue;
        f.minuto_s -= 60.0;
        const double v = std::fabs(u.velocita);
        if (s.tipo == TipoFondo::roccia && v > STRISCIA_KN && caso.uniforme() < 0.05 + 0.05 * v) {
            const Sistema sis = caso.uniforme() < 0.5 ? Sistema::timone : Sistema::sonar_prua;
            guasti_.push_back({id, static_cast<int>(sis), PERDITA_ROCCE, sis == Sistema::timone ? "Timone danneggiato dalle rocce" : "Cupola del sonar danneggiata dalle rocce"});
            u.integrita = std::max(1, u.integrita - std::max(1, static_cast<int>(std::lround(DANNO_ROCCE * u.integrita_max))));
            if (u.giocatore && !f.avvisato_rocce) messaggio_di_bordo(id, MSG_ROCCE);
            f.avvisato_rocce = true;
        }
        if (s.tipo == TipoFondo::fango && !s.prese_intasate && s.posato_s > PRESE_DOPO_S && caso.uniforme() < P_PRESE_MINUTO) {
            s.prese_intasate = true;
            guasti_.push_back({id, static_cast<int>(Sistema::propulsione), PERDITA_PRESE, "Prese a mare intasate dal fango: raffreddamento in avaria"});
            if (u.giocatore) messaggio_di_bordo(id, MSG_PRESE);
        }
    }
}

}  // namespace m688
