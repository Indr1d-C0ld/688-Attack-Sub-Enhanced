#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#include "missioni/missione.hpp"

// L'AN/BQS-15 del 688 (docs/ricerca/06_fondali_e_navigazione.md §1.1, §2 e §7; scheda S06 di docs/17): il sonar
// attivo ad alta frequenza della vela per le mine, il ghiaccio e gli ostacoli vicini. Qui le mine; il fondo e le pareti
// vanno al quadro del fondale (spazzata_fondo, missione_fondale.cpp). Tutte le
// cifre sono STIMA DI GIOCO del dossier: le vere sono classificate.
// - Una spazzata completa del settore di prua ogni SPAZZATA_S: +-60 gradi dalla rotta, da -30 a +45 in elevazione.
// - Portata: una mina ormeggiata (TS circa -15 dB) a ~1,1 km col rumore del mare forza 3; quelle sul fondo si
//   confondono con la riverberazione del fondo, ~450 m. Il rumore proprio la riduce: -30% a 15 kn, -60% oltre i 20.
// - L'eco da' distanza, azimut ed elevazione con ~5 m e ~1 grado d'errore: la posizione stimata si aggiorna a ogni
//   spazzata, finche' la mina e' nel settore.
// - Ogni spazzata e' un impulso di 205 dB a 25 kHz: chi ascolta con un sensore passivo che arriva a quella banda lo
//   sente (contro-scoperta, come il ping), nel fascio a pieno livello e fuori coi lobi laterali (-25 dB).
// - L'IA (simmetria, 30/10/2026): un sottomarino dell'IA col BQS-15 acceso tiene le sue mine scoperte come il giocatore,
//   e anche l'MG-70 e l'MG-24 dei sovietici, sonar da mine, a ogni spazzata del sonar del fondo (missione_fondale_ia.cpp);
//   il pilota le aggira al largo (evita_mine, MARGINE_MINA_M).

namespace m688 {

namespace {

constexpr double SPAZZATA_S = 8.0;                      // 06 §7: una spazzata completa ogni ~8-10 s
constexpr double SETTORE_DEG = 60.0;                    // +-60 gradi in azimut
constexpr double ELEVAZIONE_MIN = -30.0, ELEVAZIONE_MAX = 45.0;
constexpr double PORTATA_ORMEGGIATA_M = 1100.0;         // 06 §1.1: 2TL <= 134 dB con TS -15 dB
constexpr double PORTATA_FONDO_M = 450.0;               // la riverberazione del fondo (STIMA)
constexpr double ORMEGGIO_MIN_M = 2.0;                  // piu' alta sul fondo di cosi': ormeggiata
constexpr double SL_DB = 205.0, FREQUENZA_HZ = 25000.0, IMPULSO_S = 0.01;
constexpr double LOBI_DB = 25.0;                        // 06 §2: fuori fascio -20/-25 dB (a 25 dB, ~5,5 km)
constexpr double ERRORE_DISTANZA_M = 5.0, ERRORE_ANGOLO_DEG = 1.0;
constexpr double GRADI = 3.14159265358979323846 / 180.0;
constexpr int MSG_BQS15_ACCESO = 450, MSG_BQS15_MINA = 451, MSG_BQS15_SPENTO = 452;
constexpr int MSG_BQS8_ACCESO = 490, MSG_BQS8_SPENTO = 491;   // il BQS-8 degli Sturgeon (disassemblatore, docs/ricerca/34)
// l'MG-70 e l'MG-24 di un sovietico del giocatore (disassemblatore, 31/10/2026): acceso, mina, spento come il BQS-15
constexpr int MSG_MG70_ACCESO = 493, MSG_MG70_MINA = 494, MSG_MG70_SPENTO = 495;
constexpr int MSG_MG24_ACCESO = 496, MSG_MG24_MINA = 497, MSG_MG24_SPENTO = 498;
constexpr double MARGINE_MINA_M = 300.0;          // l'IA passa a questa distanza da una mina vista (STIMA)
constexpr double GUARDA_MINE_M = 2000.0;          // le mine viste cosi' avanti contano per la rotta
constexpr double MEMORIA_MINA_S = 3600.0;         // una mina vista entro un'ora (le ormeggiate non si spostano)
constexpr double PASSO_VARCO_GRADI = 5.0;         // la ricerca del varco

double normale(Casuale& c) {
    const double u1 = std::max(c.uniforme(), 1e-12), u2 = c.uniforme();
    return std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * 3.14159265358979323846 * u2);
}

double scarto(double a, double b) {   // a - b in (-180, 180]
    double d = std::fmod(a - b, 360.0);
    if (d > 180.0) d -= 360.0;
    if (d <= -180.0) d += 360.0;
    return d;
}

}  // namespace

bool Missione::ha_bqs15(const std::string& classe) { return classe.rfind("los_angeles", 0) == 0; }

double Missione::portata_bqs15(double velocita_kn, bool ormeggiata) const {
    const double v = std::abs(velocita_kn);
    // 1 fino a 5 kn, 0,7 a 15, 0,4 a 20 e oltre
    const double f = v <= 5.0 ? 1.0 : v <= 15.0 ? 1.0 - 0.03 * (v - 5.0) : v <= 20.0 ? 0.7 - 0.06 * (v - 15.0) : 0.4;
    return (ormeggiata ? PORTATA_ORMEGGIATA_M : PORTATA_FONDO_M) * f;
}

bool Missione::bqs15(int id, bool acceso) {
    const UnitaClassica* u = trova(id);
    if (!u || (!catalogo_ && !siluri_attivi_)) return false;
    const auto* g = giocatore();
    const bool del_giocatore = g && g->id == id;
    // lo Sturgeon del giocatore ha il BQS-8, il sonar da ghiaccio senza le mine (docs/ricerca/34): lo stesso ordine lo
    // accende, e fa solo le spazzate della volta (missione_ghiaccio.cpp)
    const bool bqs8 = del_giocatore && !ha_bqs15(u->classe) && par_bqs_fondo_ && !par_bqs_fondo_->mine;
    // un sovietico del giocatore ha il sonar delle mine della sua classe (l'MG-70, l'MG-24 dell'Alfa), come l'IA
    const bool sovietico = del_giocatore && !ha_bqs15(u->classe) && par_bqs_fondo_ && par_bqs_fondo_->mine;
    const bool mg24 = sovietico && par_bqs_fondo_->id == "mg24";
    if (!ha_bqs15(u->classe) && !bqs8 && !sovietico) return false;
    if (!acceso) {
        if (bqs15_.erase(id) && del_giocatore)
            messaggio_di_bordo(id, bqs8 ? MSG_BQS8_SPENTO : mg24 ? MSG_MG24_SPENTO : sovietico ? MSG_MG70_SPENTO : MSG_BQS15_SPENTO);
        return true;
    }
    if (bqs15_.try_emplace(id, tempo_).second && del_giocatore)   // la prima spazzata subito
        messaggio_di_bordo(id, bqs8 ? MSG_BQS8_ACCESO : mg24 ? MSG_MG24_ACCESO : sovietico ? MSG_MG70_ACCESO : MSG_BQS15_ACCESO);
    return true;
}

void Missione::passo_bqs15() {
    for (auto it = bqs15_.begin(); it != bqs15_.end();) {
        UnitaClassica* u = trova(it->first);
        if (!u || u->distrutta || u->persa) { it = bqs15_.erase(it); continue; }
        if (tempo_ + 1e-9 >= it->second) {
            it->second = tempo_ + SPAZZATA_S;
            // il BQS-8 dello Sturgeon: solo la volta (missione_ghiaccio.cpp); l'MG-70 o l'MG-24 di un sovietico del giocatore
            // come il BQS-15
            if (ha_bqs15(u->classe) || (u->giocatore && par_bqs_fondo_ && par_bqs_fondo_->mine)) spazzata_bqs15(*u);
        }
        ++it;
    }
}

bool Missione::rileva_mine(const UnitaClassica& u, std::vector<MinaScoperta>& dove, double scala, Casuale& caso) {
    bool nuova = false;
    for (const Mina& m : campo_minato_.mine()) {
        if (m.stato == Mina::Stato::scoppiata || m.stato == Mina::Stato::in_corsa) continue;
        const double r = distanza(u.pos, m.pos);
        const double az = scarto(rilevamento(u.pos, m.pos), u.rotta);
        if (std::abs(az) > SETTORE_DEG) continue;
        const double el = std::atan2(u.quota - m.quota, std::max(r, 1.0)) / GRADI;   // quote positive in basso
        if (el < ELEVAZIONE_MIN || el > ELEVAZIONE_MAX) continue;
        const bool ormeggiata = m.fondale - m.quota > ORMEGGIO_MIN_M;
        const double r3 = std::hypot(r, m.quota - u.quota);
        if (r3 > scala * portata_bqs15(u.velocita, ormeggiata)) continue;
        const double rs = r3 + ERRORE_DISTANZA_M * normale(caso);
        const double azs = rilevamento(u.pos, m.pos) + ERRORE_ANGOLO_DEG * normale(caso);
        const double els = el + ERRORE_ANGOLO_DEG * normale(caso);
        const double rh = rs * std::cos(els * GRADI);
        auto s = std::find_if(dove.begin(), dove.end(), [&](const MinaScoperta& x) { return x.mina == m.id; });
        if (s == dove.end()) {
            dove.push_back({m.id, {}, 0.0, tempo_, tempo_, 0.0, 0.0, ormeggiata});
            s = std::prev(dove.end());
            nuova = true;
        }
        s->pos = {u.pos.x + rh * std::sin(azs * GRADI), u.pos.y + rh * std::cos(azs * GRADI)};
        s->quota = u.quota - rs * std::sin(els * GRADI);
        s->vista_t = tempo_;
        s->distanza_m = r3;
        s->rilevamento = normalizza_360(azs);
    }
    return nuova;
}

void Missione::spazzata_bqs15(UnitaClassica& u) {
    const auto* g = giocatore();
    const bool del_giocatore = g && g->id == u.id;
    // le mine nel settore: il giocatore coi messaggi di bordo, l'IA per il suo pilota
    // il sovietico del giocatore: il suo MG-70 o MG-24 con la portata ridotta della sua voce (bqs15_fondo di acustica.toml)
    const bool sovietico = del_giocatore && !ha_bqs15(u.classe) && par_bqs_fondo_ && par_bqs_fondo_->mine;
    if (del_giocatore) {
        const bool nuova = rileva_mine(u, mine_scoperte_, sovietico ? par_bqs_fondo_->fattore_portata : 1.0, caso_bqs15_);
        if (nuova)
            messaggio_di_bordo(u.id, !sovietico ? MSG_BQS15_MINA : par_bqs_fondo_->id == "mg24" ? MSG_MG24_MINA : MSG_MG70_MINA);
        spazzata_fondo(u);   // la stessa spazzata sul fondo e sulle pareti, per il quadro del fondale (missione_fondale.cpp)
    } else if (u.categoria == "sottomarino") {
        rileva_mine(u, mine_scoperte_ia_[u.id], 1.0, caso_mine_ia_);   // l'IA: per il suo pilota (evita_mine)
    }
    // la contro-scoperta: l'impulso ai sensori passivi degli altri, nel fascio o coi lobi laterali
    if (!catalogo_) return;
    emetti_impulso(u.id, sovietico ? par_bqs_fondo_->emissione : "bqs15", SL_DB, FREQUENZA_HZ, SETTORE_DEG, LOBI_DB);   // e agli intercettatori (E)
    auto sp = sorgente_acustica(u);
    if (!sp) return;
    sp->ping_hz = FREQUENZA_HZ;
    sp->ping_s = IMPULSO_S;
    for (auto& [idb, b] : unita_) {
        if (idb == u.id || b.distrutta || b.persa) continue;
        const bool nel_fascio = std::abs(scarto(rilevamento(u.pos, b.pos), u.rotta)) <= SETTORE_DEG;
        sp->ping_db = SL_DB - (nel_fascio ? 0.0 : LOBI_DB);
        intercetta_impulso(u, b, *sp, ambiente_acustico(u.pos, b.pos));
    }
}

const std::vector<Missione::MinaScoperta>* Missione::mine_scoperte_ia(int id) const {
    const auto it = mine_scoperte_ia_.find(id);
    return it == mine_scoperte_ia_.end() ? nullptr : &it->second;
}

// La rotta dell'IA al largo delle mine che ha visto (entro un'ora, entro GUARDA_MINE_M): se la rotta passa a meno di
// MARGINE_MINA_M da una di loro, la rotta libera piu' vicina, cioe' il tratto di GUARDA_MINE_M che resta a MARGINE_MINA_M da
// tutte (il varco in una fila; principale, 30/10/2026: il pilota passava fra due mine a 200 m); provata a passi di
// PASSO_VARCO_GRADI da una parte e dall'altra, fino a girare indietro. Dentro il margine d'una mina: via da lei. Nessuna
// mina sulla rotta: nullopt
std::optional<double> Missione::evita_mine(const UnitaClassica& u, double rotta) const {
    const auto it = mine_scoperte_ia_.find(u.id);
    if (it == mine_scoperte_ia_.end()) return std::nullopt;
    std::vector<Vec2> vicine;
    const MinaScoperta* dentro = nullptr;
    double dv = 1e18;
    const auto& tutte = campo_minato_.mine();
    for (const MinaScoperta& s : it->second) {
        if (tempo_ - s.vista_t > MEMORIA_MINA_S) continue;
        if (const auto m = std::find_if(tutte.begin(), tutte.end(), [&](const Mina& x) { return x.id == s.mina; });
            m != tutte.end() && m->stato == Mina::Stato::scoppiata)
            continue;
        const double d = distanza(u.pos, s.pos);
        if (d > GUARDA_MINE_M + MARGINE_MINA_M) continue;
        vicine.push_back(s.pos);
        if (d < MARGINE_MINA_M && d < dv) { dv = d; dentro = &s; }
    }
    if (vicine.empty()) return std::nullopt;
    if (dentro) return normalizza_360(rilevamento(u.pos, dentro->pos) + 180.0);   // dentro il margine: via
    // il tratto di GUARDA_MINE_M lungo h resta a MARGINE_MINA_M da tutte?
    const auto libera = [&](double h) {
        const double r = h * GRADI, ux = std::sin(r), uy = std::cos(r);
        for (const Vec2& q : vicine) {
            const double px = q.x - u.pos.x, py = q.y - u.pos.y;
            const double t = std::clamp(px * ux + py * uy, 0.0, GUARDA_MINE_M);
            if (std::hypot(px - t * ux, py - t * uy) < MARGINE_MINA_M) return false;
        }
        return true;
    };
    if (libera(rotta)) return std::nullopt;
    for (double o = PASSO_VARCO_GRADI; o <= 180.0 + 1e-9; o += PASSO_VARCO_GRADI)
        for (const double h : {rotta + o, rotta - o})
            if (libera(normalizza_360(h))) return normalizza_360(h);
    return normalizza_360(rotta + 180.0);   // chiuso da ogni parte: indietro
}

}  // namespace m688
