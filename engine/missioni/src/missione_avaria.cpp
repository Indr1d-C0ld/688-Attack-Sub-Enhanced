#include <algorithm>
#include <cmath>
#include <string>

#include "missioni/missione.hpp"

// Unita' dell'IA in avaria, incagliate e rimorchiate (M6 del dossier docs/ricerca/23_episodi_guerra_fredda.md: E07 il
// K-219 rimorchiato, E14 l'U-137 sugli scogli, E19 e E23; schede S04 e S11 di docs/17).
// - In avaria la propulsione e' persa: l'unita' rallenta fino a fermarsi, qualunque cosa ordini il suo script, e ogni
//   RUMORE_MIN_S-RUMORE_MAX_S fa un rumore di riparazione (attrezzi sullo scafo: un transitorio di RUMORE_S per i
//   sonar). STIMA di gioco.
// - Incagliata: resta nel punto dell'incaglio finche' non la rimorchiano.
// - Rimorchio: il rimorchiato tiene il cavo teso sulla scia del rimorchiatore (a `cavo_m`, con la prua verso di lui);
//   col cavo lento resta fermo. Il rimorchiatore non supera `velocita_kn`. Un sottomarino rimorchiato e' in
//   superficie (l'U-137 e il K-219). Il moto orizzontale del rimorchiato non passa per la navigazione: niente incagli
//   o urti col fondo mentre lo tirano via.

namespace m688 {

namespace {

constexpr double RUMORE_MIN_S = 90.0, RUMORE_MAX_S = 300.0, RUMORE_S = 3.0;
constexpr double RIMORCHIO_MAX_KN = 12.0;   // oltre, il cavo non tiene (STIMA)

bool scafo(const UnitaClassica& u) {
    return !u.giocatore && !(u.flag & 0x20) && (u.categoria == "nave" || u.categoria == "sottomarino");
}

}  // namespace

bool Missione::avaria(int id, bool si) {
    UnitaClassica* u = trova(id);
    if (!u || !scafo(*u)) return false;
    if (!si) {
        const auto it = avarie_.find(id);
        if (it == avarie_.end()) return true;
        it->second.propulsione_persa = false;
        if (!it->second.incagliata) avarie_.erase(it);
        return true;
    }
    AvariaIa& a = avarie_[id];
    if (a.propulsione_persa) return true;
    a.propulsione_persa = true;
    a.prossimo_rumore = tempo_ + caso_avarie_.uniforme(RUMORE_MIN_S, RUMORE_MAX_S);
    avarie_iniziate_.push_back({tempo_, id});
    return true;
}

bool Missione::incaglia(int id) {
    UnitaClassica* u = trova(id);
    if (!u || !scafo(*u) || rimorchiatore_di(id) >= 0) return false;
    AvariaIa& a = avarie_[id];
    a.incagliata = true;
    a.punto = u->pos;
    u->velocita = u->velocita_ordinata = 0.0;
    return true;
}

bool Missione::in_avaria(int id) const {
    const auto it = avarie_.find(id);
    return it != avarie_.end() && it->second.propulsione_persa;
}

bool Missione::incagliata_ia(int id) const {
    const auto it = avarie_.find(id);
    return it != avarie_.end() && it->second.incagliata;
}

int Missione::rimorchiatore_di(int id) const {
    for (const auto& r : rimorchi_) if (r.rimorchiato == id) return r.rimorchiatore;
    return -1;
}

bool Missione::rimorchia(int rimorchiatore, int rimorchiato, double velocita_kn, double cavo_m) {
    const UnitaClassica* t = trova(rimorchiatore);
    UnitaClassica* r = trova(rimorchiato);
    if (!t || !r || rimorchiatore == rimorchiato || !scafo(*t) || !scafo(*r) || t->categoria != "nave") return false;
    for (const auto& x : rimorchi_)   // una sola catena: niente rimorchi di rimorchi
        if (x.rimorchiato == rimorchiato || x.rimorchiato == rimorchiatore || x.rimorchiatore == rimorchiato) return false;
    if (const auto it = avarie_.find(rimorchiato); it != avarie_.end()) {   // il rimorchio la stacca dagli scogli
        it->second.incagliata = false;
        if (!it->second.propulsione_persa) avarie_.erase(it);
    }
    r->incagliata = false;
    r->navigazione = false;
    rimorchi_.push_back({rimorchiatore, rimorchiato, std::clamp(velocita_kn, 1.0, RIMORCHIO_MAX_KN), std::max(cavo_m, 50.0)});
    eventi_rimorchio_.push_back({tempo_, rimorchiatore, rimorchiato, true});
    return true;
}

bool Missione::molla_rimorchio(int id) {
    const auto it = std::find_if(rimorchi_.begin(), rimorchi_.end(), [&](const Rimorchio& x) { return x.rimorchiato == id || x.rimorchiatore == id; });
    if (it == rimorchi_.end()) return false;
    eventi_rimorchio_.push_back({tempo_, it->rimorchiatore, it->rimorchiato, false});
    if (UnitaClassica* r = trova(it->rimorchiato)) r->velocita = r->velocita_ordinata = 0.0;
    rimorchi_.erase(it);
    return true;
}

void Missione::passo_avarie(double dt) {
    for (auto it = relitti_.begin(); it != relitti_.end();) {   // fermi sul fondo, senza alberi ne' contatti
        UnitaClassica* u = trova(it->first);
        if (!u || u->distrutta || u->persa) { it = relitti_.erase(it); continue; }
        u->pos = it->second;
        u->velocita = u->velocita_ordinata = 0.0;
        u->navigazione = false;
        u->flag54 = 0;
        u->contatti.clear();
        if (fondale_) {
            const double fondo = profondita(u->pos);
            if (fondo > 0.0) u->quota = u->quota_ordinata = fondo;
        }
        u->sul_fondo = true;
        ++it;
    }
    for (auto it = avarie_.begin(); it != avarie_.end();) {
        UnitaClassica* u = trova(it->first);
        if (!u || u->distrutta || u->persa) { it = avarie_.erase(it); continue; }
        AvariaIa& a = it->second;
        if (a.incagliata) {
            u->pos = a.punto;
            u->velocita = u->velocita_ordinata = 0.0;
        } else if (a.propulsione_persa && rimorchiatore_di(u->id) < 0) {
            u->velocita_ordinata = 0.0;   // rallenta con la sua accelerazione: l'abbrivio
            u->velocita = std::max(0.0, u->velocita - u->accelerazione * dt);
        }
        if (a.propulsione_persa && tempo_ >= a.prossimo_rumore) {   // il rumore di riparazione
            evento_acustico(u->id, RUMORE_S);
            a.prossimo_rumore = tempo_ + caso_avarie_.uniforme(RUMORE_MIN_S, RUMORE_MAX_S);
        }
        ++it;
    }
    for (auto it = rimorchi_.begin(); it != rimorchi_.end();) {
        UnitaClassica* t = trova(it->rimorchiatore);
        UnitaClassica* r = trova(it->rimorchiato);
        if (!t || !r || t->distrutta || t->persa || r->distrutta || r->persa || t->affondamento > 0 || r->affondamento > 0) {
            eventi_rimorchio_.push_back({tempo_, it->rimorchiatore, it->rimorchiato, false});
            if (r) r->velocita = r->velocita_ordinata = 0.0;
            it = rimorchi_.erase(it);
            continue;
        }
        t->velocita_ordinata = std::min(t->velocita_ordinata, it->velocita_kn);
        t->velocita = std::min(t->velocita, it->velocita_kn);
        const double d = distanza(r->pos, t->pos);
        if (d > it->cavo_m) {   // il cavo teso: sulla congiungente, a cavo_m dal rimorchiatore
            const double k = it->cavo_m / d;
            r->pos = {t->pos.x + (r->pos.x - t->pos.x) * k, t->pos.y + (r->pos.y - t->pos.y) * k};
            r->velocita = t->velocita;
        } else {
            r->velocita = 0.0;
        }
        r->velocita_ordinata = r->velocita;
        if (d > 1.0) r->rotta = r->rotta_ordinata = normalizza_360(rilevamento(r->pos, t->pos));
        if (r->categoria == "sottomarino") {   // in superficie
            r->quota_ordinata = 0.0;
            r->quota = std::max(0.0, r->quota - std::max(r->rateo_quota_modello, 0.5) * dt);
        }
        ++it;
    }
}

}  // namespace m688
