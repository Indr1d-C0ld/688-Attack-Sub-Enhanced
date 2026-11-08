// Traffico civile nella missione dell'Enhanced (docs/20_sonar_blocco1.md §3, contratto concordato col disassemblatore;
// modulo puro nucleo/traffico.hpp). Il modulo decide, la missione applica:
// - a ogni passo Traffico::avanza sulla posizione vera del giocatore; per ogni entrata un'unita' vera (id da 10000,
//   neutrale e non combattente: flag 42h, quindi mai osservatore), per ogni uscita togli (non e' un affondamento);
//   poi pos, rotta e velocita' copiate dalle navi;
// - una nave colpita, affondata, distrutta o incagliata esce dal traffico (Traffico::perduta) e resta un'unita' come
//   le altre;
// - un'esplosione (un colpo del passo) entro FUGA_M fa fuggire le navi vicine (Traffico::fuggi);
// - le navi sono solo sorgenti: la firma dell'individuo (sorgente_acustica), il livello A oltre 30 km
//   (ascolta_con_acustica), nessuno script le sceglie come bersaglio (scelta_bersaglio), l'IA non le attacca per
//   l'errore d'identificazione ad A3 (armi_libere_ia).
// Spento nella Missione finche' nessuno chiama usa_traffico: il gioco lo accende nell'Enhanced (opzione `traffico`,
// fuori dalle scene di prova). La chiave di missione `traffico = false` lo spegne sempre.

#include <cmath>

#include "missioni/missione.hpp"

namespace m688 {

namespace {

constexpr double FUGA_M = 10000.0, FUGA_S = 1800.0;   // chi sente un'esplosione entro 10 km scappa per mezz'ora (STIMA)
constexpr std::uint8_t FLAG_TRAFFICO = 0x42;           // nave (02h), non combattente (40h), neutrale (bit 18h a zero)
constexpr double TRAFFICO_PASSO_S = 1.0;

}  // namespace

void Missione::passo_traffico(double h) {
    // una volta al secondo: le navi vanno a 10-20 nodi, e il modulo costa 0,2 ms a passo con 40 navi
    traffico_dt_ += h;
    if (traffico_ && traffico_dt_ < TRAFFICO_PASSO_S - 1e-9) return;
    const double dt = traffico_dt_;
    traffico_dt_ = 0.0;
    const auto* g = giocatore();
    if (!g || g->distrutta || g->persa) return;
    if (!traffico_) {
        if (traffico_tentato_ || cartella_dati_traffico_.empty()) return;
        traffico_tentato_ = true;   // un teatro senza file non ha traffico: si prova una volta
        try {
            rotte_traffico_ = std::make_unique<RotteTraffico>(carica_traffico(file_traffico(cartella_dati_traffico_, teatro_)));
        } catch (const std::exception&) {
            return;
        }
        traffico_.emplace(*rotte_traffico_, *catalogo_, mese_acustica(), seme_);
    }
    // le navi perdute nella missione: escono dal traffico e restano unita' come le altre
    for (auto it = id_traffico_.begin(); it != id_traffico_.end();) {
        const UnitaClassica* u = trova(it->first);
        if (u && !u->persa && !u->distrutta && u->colpo == 0 && u->affondamento == 0 && !u->incagliata) { ++it; continue; }
        if (!u || !u->persa) traffico_->perduta(it->second);   // persa senza uscita: l'ha tolta la missione
        if (u) raggi_.dimentica(u->id);   // fuori dall'insieme condiviso: d'ora in poi il suo campo dei raggi
        traffico_id_.erase(it->second);
        it = id_traffico_.erase(it);
    }
    // le esplosioni del passo: chi e' vicino scappa
    for (; colpi_visti_traffico_ < colpi_.size(); ++colpi_visti_traffico_) {
        const Colpo& c = colpi_[colpi_visti_traffico_];
        for (const auto& n : traffico_->navi())
            if (distanza(n.pos, c.pos) <= FUGA_M) traffico_->fuggi(n.indice, c.pos, tempo_ + FUGA_S);
    }
    const double ora_locale = std::fmod((ora_utc_s_ + tempo_) / 3600.0 + longitudine_ / 15.0 + 48.0, 24.0);
    const double dt_mondo = dt * m_s_per_nodo_ / NODO;   // col moto classico le navi vanno come le altre unita'
    const Ricambio r = traffico_->avanza(tempo_, dt_mondo, g->pos, ora_locale);
    for (const int i : r.uscite) {
        const auto t = traffico_id_.find(i);
        if (t == traffico_id_.end()) continue;
        if (UnitaClassica* u = trova(t->second)) togli(*u);
        id_traffico_.erase(t->second);
        traffico_id_.erase(t);
    }
    for (const int i : r.entrate) {
        const NaveTraffico* n = traffico_->nave(i);
        if (!n || traffico_id_.count(i)) continue;
        UnitaClassica u;
        u.id = prossimo_id_traffico_++;
        u.classe = n->classe;
        u.categoria = "nave";
        u.flag = FLAG_TRAFFICO;
        u.pos = n->pos;
        u.rotta = u.rotta_ordinata = n->rotta;
        u.velocita = u.velocita_ordinata = n->velocita_kn;
        u.avanti_tutta = std::max(n->velocita_kn, 1.0);
        u.sonar = 0;   // solo sorgente
        ++create_;     // gli osservatori valutano anche lei (DS:1F56: le unita' create nella missione)
        unita_[u.id] = u;
        id_traffico_[u.id] = i;
        traffico_id_[i] = u.id;
        raggi_.condividi(u.id);   // i campi dei raggi condivisi fra le navi del traffico (nucleo/acustica.hpp)
    }
    for (const auto& n : traffico_->navi()) {
        const auto t = traffico_id_.find(n.indice);
        if (t == traffico_id_.end()) continue;
        UnitaClassica* u = trova(t->second);
        if (!u) continue;
        u->pos = n.pos;
        u->rotta = u->rotta_ordinata = n.rotta;
        u->velocita = u->velocita_ordinata = n.velocita_kn;
    }
}

}  // namespace m688
