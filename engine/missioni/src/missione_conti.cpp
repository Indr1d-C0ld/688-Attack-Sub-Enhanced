// I conti della missione del giocatore per gli achievement e le statistiche (docs/26_achievement_statistiche_carriera.md
// §2-3): quelli che il registro non vede da solo perche' sono grandezze continue (l'acqua sotto la chiglia, il mare) o
// stati (il passaggio in un campo di mine). Gli altri (radar acceso, allarmi dell'intercettatore, verifiche radio) li
// segnano gli ordini e i passi che li vivono; il registro li legge come eventi.

#include <algorithm>
#include <cmath>

#include "missioni/missione.hpp"

namespace m688 {

namespace {

constexpr double SOTTO_CHIGLIA_M = 20.0;      // l'achievement «Sotto la chiglia» (docs/26 §2)
constexpr double MINE_DENTRO_M = 500.0, MINE_FUORI_M = 1000.0;   // il passaggio in un campo: entro, e di nuovo fuori (STIMA)
// il secondo giro degli achievement (nota disassemblatore-achievement-31-10-proposta.md §8)
constexpr double EMERSO_M = 2.0;              // superficie sotto 2 m, come le statistiche
constexpr double NOTTE_LUCE = 0.2;            // la luce sotto cui e' notte (come il libretto e l'artiglieria)
constexpr double CODA_M = 3.0 * 1852.0, CODA_SETTORE_GRADI = 40.0;   // in coda: entro 3 nm, nel settore di poppa
constexpr double VICINO_M = 457.0;            // 500 yd
constexpr double TRAINO_VICINO_M = 5000.0;

bool arma(const UnitaClassica& u) { return (u.flag & 0x20) != 0 || u.categoria == "arma"; }

}  // namespace

void Missione::passo_conti(double dt) {
    const UnitaClassica* g = giocatore();
    if (!g || g->distrutta || g->persa) return;
    conti_.mare_max = std::max(conti_.mare_max, stato_mare());
    if (fondale_) {
        const double fondo = profondita(g->pos);
        if (std::isfinite(fondo) && fondo - g->quota < SOTTO_CHIGLIA_M && g->quota > 1.0) conti_.sotto_chiglia_20_s += dt;
    }
    // il ghiaccio: il tempo sotto la banchisa, le emersioni dentro (dove si emerge: acqua libera o nilas)
    if (sotto_banchisa(g->id)) conti_.sotto_banchisa_s += dt;
    const bool emerso = g->quota < EMERSO_M;
    if (emerso && !conti_.emerso && copertura_ghiaccio(g->pos) > 0.0) ++conti_.emersioni_ghiaccio;
    conti_.emerso = emerso;
    // le luci proprie accese in superficie di notte: un episodio a ogni accensione (l'esposizione della carriera)
    const bool luci = emerso && luce_periscopi() < NOTTE_LUCE && luci_giocatore_accese();
    if (luci && !conti_.luci_notte) ++conti_.luci_accese_notte;
    conti_.luci_notte = luci;
    // in coda a un sottomarino straniero e entro 500 yd; le navi che filano il VDS o la schiera passate vicino
    bool coda = false, vicino = false;
    for (const auto& [id, u] : unita_) {
        if (u.giocatore || u.distrutta || u.persa || arma(u)) continue;
        const double d = distanza(g->pos, u.pos);
        if (u.categoria == "nave") {
            if (d <= TRAINO_VICINO_M && (vds_in_acqua(id) || (u.flag54 & 1))) conti_.navi_traino_vicine.insert(id);
            continue;
        }
        if (u.categoria != "sottomarino" || (u.flag & 0x18) == (g->flag & 0x18)) continue;
        vicino = vicino || d <= VICINO_M;
        if (coda || d > CODA_M || !g->contatti.count(id)) continue;
        if (std::fabs(normalizza_180(rilevamento(u.pos, g->pos) - (u.rotta + 180.0))) > CODA_SETTORE_GRADI) continue;
        coda = std::none_of(classificazioni_ia_.begin(), classificazioni_ia_.end(),
                            [&](const ClassificazioneIa& k) { return k.unita == id && k.sorgente == g->id; });
    }
    conti_.coda_ora_s = coda ? conti_.coda_ora_s + dt : 0.0;
    conti_.vicino_ora_s = vicino ? conti_.vicino_ora_s + dt : 0.0;
    conti_.in_coda_s = std::max(conti_.in_coda_s, conti_.coda_ora_s);
    conti_.entro_500yd_s = std::max(conti_.entro_500yd_s, conti_.vicino_ora_s);
    // il passaggio in un campo di mine ostili (non posate dalla parte del giocatore): conta se col BQS-15 sempre acceso
    // e senza danni
    if (campo_minato_.vuoto()) return;
    // le mine scoperte: quanto ci si e' avvicinati dopo (aggirata se mai entro 200 m)
    for (const MinaScoperta& x : mine_scoperte_)
        for (const Mina& k : campo_minato_.mine())
            if (k.id == x.mina) {
                const double d = distanza(g->pos, k.pos);
                const auto it = conti_.mine_vicine_m.find(x.mina);
                if (it == conti_.mine_vicine_m.end()) conti_.mine_vicine_m[x.mina] = d;
                else it->second = std::min(it->second, d);
            }
    double vicina = 1e18;
    for (const Mina& k : campo_minato_.mine())
        if (k.posatore != g->id && k.lato != (g->flag & 0x18) && (k.stato == Mina::Stato::armata || k.stato == Mina::Stato::armandosi))
            vicina = std::min(vicina, distanza(g->pos, k.pos));
    if (!mine_dentro_ && vicina <= MINE_DENTRO_M) {
        mine_dentro_ = true;
        mine_bqs_ = bqs15_acceso(g->id);
        mine_integrita_ = g->integrita;
    } else if (mine_dentro_) {
        mine_bqs_ = mine_bqs_ && bqs15_acceso(g->id);
        if (vicina > MINE_FUORI_M) {
            mine_dentro_ = false;
            if (mine_bqs_ && g->integrita >= mine_integrita_) conti_.attraversamenti_mine.push_back(tempo_);
        }
    }
}

}  // namespace m688
