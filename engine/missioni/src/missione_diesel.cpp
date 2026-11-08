#include <algorithm>
#include <cmath>
#include <string>

#include "missioni/missione.hpp"
#include "missioni/periscopi_missione.hpp"
#include "nucleo/evasione.hpp"

// La batteria dei sottomarini diesel dell'IA (M6 del dossier docs/ricerca/23_episodi_guerra_fredda.md: E02, i Foxtrot
// di Cuba costretti a emergere; scheda S03 «Quarantena» di docs/17). Solo con la regola di simulazione.
// - La carica (0-1) si consuma con P = P_bordo + k v^3, con P_bordo e k tali che la batteria piena duri `ore_eco` alla
//   velocita' economica e un'ora a tutta forza (STIMA: Foxtrot 175 h a 2 kn, Kilo 133 h a 3 kn; 22 nodi e un'ora sono
//   l'ordine di grandezza delle fonti aperte).
// - Sotto SNORKEL_SOTTO il battello sale a quota periscopica e ricarica a 0,1 l'ora coi diesel (rumore per i sonar: la
//   sorgente ha `snorkel`), finche' non arriva a SNORKEL_FINO. Non lo fa se e' in caccia (evasione, un contatto ostile
//   con la posizione) finche' la carica non scende a EMERGENZA.
// - A EMERGENZA emerge per forza: in superficie a 6 nodi coi diesel, finche' non ha ricaricato SNORKEL_FINO.
// - Sotto il pack (runtime, 29/10/2026; simmetria del ghiaccio, docs/18): lo snorkel e l'emersione forzata solo in acqua
//   libera. Sotto la banchisa il battello resta a QUOTA_SOTTO_PACK_M e va al rifugio con la batteria; li' sale. Il
//   rifugio: la polynya piu' recente nel suo quadro della volta (il sondatore alto, anche dei diesel senza il sonar del
//   fondo: missione_ghiaccio.cpp; in silenzio non sonda); se no il margine del pack che sa dalle carte del ghiaccio (la
//   copertura del meteo sotto MARGINE_COPERTURA, entro MARGINE_MAX_M). Sotto il ghiaccio non si ricarica. Nell'acqua
//   libera del pack sale e ci resta, piano (FERMO_KN) sul punto dove l'ha trovata; se il pack la richiude, torna sotto.

namespace m688 {

namespace {

constexpr double CARICA_INIZIALE = 0.8;
constexpr double SNORKEL_SOTTO = 0.3, SNORKEL_FINO = 0.9, EMERGENZA = 0.05;
constexpr double RICARICA_ORA = 0.1;            // per ora di snorkel o in superficie (STIMA: 8-10 ore per la carica)
constexpr double VELOCITA_DIESEL_KN = 6.0;      // snorkel e superficie (STIMA)
constexpr double ORE_TUTTA = 1.0;               // a tutta forza la batteria piena dura un'ora (STIMA)
constexpr double ORA_S = 3600.0;
constexpr double QUOTA_SOTTO_PACK_M = 50.0;     // sotto le chiglie del primo anno (STIMA)
constexpr double RIFUGIO_OGNI_S = 600.0;        // la ricerca del rifugio, di nuovo (il pack deriva)
constexpr double RIFUGIO_ARRIVO_M = 300.0;      // arrivato e ancora sotto il ghiaccio: ne cerca un altro
constexpr double POLYNYA_RAGGIO_M = 20000.0, POLYNYA_ETA_S = 1800.0;
constexpr double MARGINE_COPERTURA = 0.3, MARGINE_PASSO_M = 2000.0, MARGINE_MAX_M = 100000.0;   // STIMA
constexpr double FERMO_KN = 1.5;                // nella polynya: quanto basta per governare (STIMA)

struct Autonomia { double kn, ore, massima_kn; };
Autonomia autonomia(const std::string& classe) {   // STIMA, fonti aperte (Jane's): miglia a velocita' economica
    if (classe == "kilo") return {3.0, 133.0, 17.0};      // 400 nm a 3 kn, 17 kn immerso
    return {2.0, 175.0, 16.0};                            // Foxtrot: 350 nm a 2 kn, 16 kn immerso
}

}  // namespace

void Missione::batterie_enhanced() {
    if (batterie_attive_) return;
    batterie_attive_ = true;
    for (const auto& [id, u] : unita_) {
        if (u.categoria != "sottomarino" || u.giocatore || !battello_diesel(u.classe)) continue;
        BatteriaIa b;
        const Autonomia a = autonomia(u.classe);
        b.v_eco_kn = a.kn;
        b.ore_eco = a.ore;
        b.v_max_kn = a.massima_kn;
        const auto it = batterie_iniziali_.find(id);
        b.carica = it != batterie_iniziali_.end() ? it->second : CARICA_INIZIALE;
        batterie_[id] = b;
    }
}

double Missione::batteria(int id) const {
    const auto it = batterie_.find(id);
    return it == batterie_.end() ? -1.0 : it->second.carica;
}
bool Missione::snorkel(int id) const {
    const auto it = batterie_.find(id);
    return it != batterie_.end() && it->second.snorkel;
}
bool Missione::emersione_forzata(int id) const {
    const auto it = batterie_.find(id);
    return it != batterie_.end() && it->second.emersa;
}

std::optional<Missione::ComandoManovra> Missione::guida_batteria(const UnitaClassica& u) const {
    const auto it = batterie_.find(u.id);
    if (it == batterie_.end()) return std::nullopt;
    const BatteriaIa& b = it->second;
    if ((b.emersa || b.snorkel) && sotto_banchisa(u.id)) {   // sotto il pack: in profondita' verso il rifugio, a batteria
        const double quota = std::max(u.quota_ordinata, QUOTA_SOTTO_PACK_M);
        return ComandoManovra{b.rifugio ? rilevamento(u.pos, *b.rifugio) : u.rotta_ordinata, b.v_eco_kn, quota};
    }
    if ((b.emersa || b.snorkel) && b.rifugio)   // nell'acqua libera del pack: sale e ci resta, piano
        return ComandoManovra{distanza(u.pos, *b.rifugio) > 50.0 ? rilevamento(u.pos, *b.rifugio) : u.rotta, FERMO_KN,
                              b.emersa ? 0.0 : QUOTA_PERISCOPICA_MISSIONE_M};
    if (b.emersa) return ComandoManovra{u.rotta_ordinata, VELOCITA_DIESEL_KN, 0.0};
    if (b.snorkel) return ComandoManovra{u.rotta_ordinata, VELOCITA_DIESEL_KN, QUOTA_PERISCOPICA_MISSIONE_M};
    if (b.carica <= EMERGENZA * 2.0) return ComandoManovra{u.rotta_ordinata, b.v_eco_kn, u.quota_ordinata};   // si trascina
    return std::nullopt;
}

void Missione::passo_batterie(double dt) {
    for (auto it = batterie_.begin(); it != batterie_.end();) {
        const auto ui = unita_.find(it->first);
        if (ui == unita_.end() || ui->second.distrutta || ui->second.persa) { it = batterie_.erase(it); continue; }
        UnitaClassica& u = ui->second;
        BatteriaIa& b = it->second;
        const bool in_superficie = u.quota <= 1.0;
        const bool a_quota_snorkel = u.quota <= QUOTA_PERISCOPICA_MISSIONE_M + 1.0;
        const bool sotto_pack = sotto_banchisa(u.id);
        if (!sotto_pack && ((b.snorkel && a_quota_snorkel) || (b.emersa && in_superficie) || (in_superficie && !b.snorkel && !b.emersa))) {
            b.carica = std::min(1.0, b.carica + RICARICA_ORA * dt / ORA_S);   // diesel: si ricarica
        } else {
            // P(v) = P_bordo + k v^3, la batteria piena (1) dura ore_eco a v_eco e ORE_TUTTA a tutta forza
            const double v1 = b.v_eco_kn, v2 = std::max(b.v_max_kn, v1 + 1.0);
            const double k = (1.0 / ORE_TUTTA - 1.0 / b.ore_eco) / (v2 * v2 * v2 - v1 * v1 * v1);
            const double bordo = std::max(0.0, 1.0 / b.ore_eco - k * v1 * v1 * v1);
            const double v = std::max(0.0, u.velocita);
            b.carica = std::max(0.0, b.carica - (bordo + k * v * v * v) * dt / ORA_S);
        }
        // in caccia: evasione in corso o un contatto ostile che ne conosce la posizione
        bool caccia = asw_ && guida_evasione(u);
        for (const auto& [oid, o] : unita_)
            if (!caccia && ostili(o, u) && !o.distrutta && !o.persa)
                if (const auto c = o.contatti.find(u.id); c != o.contatti.end() && c->second.posizione_nota && tempo_ - c->second.ultima_scoperta < 600.0)
                    caccia = true;
        if (b.emersa) {
            if (b.carica >= SNORKEL_FINO) b.emersa = false;   // si puo' immergere di nuovo
        } else if (b.carica <= EMERGENZA) {
            b.emersa = true;
            b.snorkel = false;
            emersioni_forzate_.push_back({tempo_, u.id});
        } else if (b.snorkel) {
            if (b.carica >= SNORKEL_FINO || caccia) b.snorkel = false;
        } else if (b.carica < SNORKEL_SOTTO && !caccia) {
            b.snorkel = true;
        }
        // sotto il pack il rifugio dove salire: cercato all'inizio, ogni RIFUGIO_OGNI_S e quando c'e' arrivato senza
        // trovarvi l'acqua libera; fuori dal ghiaccio non serve
        if ((b.snorkel || b.emersa) && sotto_pack) {
            if (!b.rifugio || tempo_ - b.rifugio_t >= RIFUGIO_OGNI_S || distanza(u.pos, *b.rifugio) < RIFUGIO_ARRIVO_M) {
                b.rifugio = acqua_libera_ia(u);
                b.rifugio_t = tempo_;
            }
        } else if ((b.snorkel || b.emersa) && ghiaccio_ && copertura_ghiaccio(u.pos) > 0.0) {
            // nell'acqua libera del pack: il rifugio diventa il punto dove l'ha trovata, e li' resta
            if (!b.rifugio || distanza(u.pos, *b.rifugio) > RIFUGIO_ARRIVO_M) b.rifugio = u.pos;
        } else {
            b.rifugio.reset();
        }
        ++it;
    }
}

std::optional<Vec2> Missione::acqua_libera_ia(const UnitaClassica& u) const {
    if (!ghiaccio_) return std::nullopt;
    // la polynya vista dal suo sondatore alto (il quadro della volta, nel riferimento della stima): riportata nel vero
    if (const auto v = volta_.find(u.id); v != volta_.end()) {
        const auto c = v->second.quadro.polynye(posizione_stimata(u.id), POLYNYA_RAGGIO_M, tempo_, POLYNYA_ETA_S);
        if (!c.empty()) {
            const Vec2 e = errore_navigazione(u.id);
            return Vec2{c.front().centro.x - e.x, c.front().centro.y - e.y};
        }
    }
    // il margine del pack: a cerchi di MARGINE_PASSO_M, un punto ogni MARGINE_PASSO_M d'arco
    for (double r = MARGINE_PASSO_M; r <= MARGINE_MAX_M; r += MARGINE_PASSO_M) {
        const int n = std::max(16, static_cast<int>(std::ceil(2.0 * PI * r / MARGINE_PASSO_M)));
        for (int k = 0; k < n; ++k) {
            const double a = radianti(360.0 * k / n);
            if (const Vec2 q{u.pos.x + r * std::sin(a), u.pos.y + r * std::cos(a)}; copertura_ghiaccio(q) <= MARGINE_COPERTURA) return q;
        }
    }
    return std::nullopt;
}

}  // namespace m688
