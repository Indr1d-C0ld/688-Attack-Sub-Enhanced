// Il libretto di riconoscimento al periscopio dell'Enhanced (docs/25_libretto_riconoscimento.md §3.2, §3.4, §3.5). Il
// collegamento nel motore:
// - l'ordine identifica_visiva: il giocatore attribuisce dalla tavola una classe al contatto, che puo' essere sbagliata.
//   Il registro la scrive come classificazione visiva, distinta da quella del sonarista; il pannello dei missili
//   (classi_credute) e le regole d'ingaggio del lancio (giudica_lancio_giocatore) la leggono;
// - l'ordine misura_telemetro: la distanza letta col telemetro e l'altezza del quadrante (misure_telemetro, con la
//   distanza vera per il registro e il debriefing: mai nei messaggi dell'equipaggio);
// - l'ordine osserva_aob: l'angolo sulla prora stimato dalle sagome, con la sua incertezza. La rotta del bersaglio e'
//   rilevamento + 180 - AOB (osservazioni_aob, per il MATE e la TMA);
// - l'identificazione visiva dell'IA, per la simmetria: su ogni contatto «vista» (o radar entro la portata della
//   vista: la vedetta guarda dove dice il radar) una classe, sbagliata con una P che
//   cresce con la distanza sulla portata della vista (foschia compresa), la notte, l'aspetto di prua o di poppa e la
//   sola vela dei sottomarini. Vale anche fra unita' dell'IA; lato_creduto la preferisce al sonarista finche' e'
//   fresca. Le costanti sono STIME di gioco.
// Il classico non lo usa: servono i periscopi dell'Enhanced.

#include <algorithm>
#include <cmath>
#include <set>

#include "missioni/missione.hpp"
#include "nucleo/aob_ia.hpp"

namespace m688 {

namespace {

constexpr double LIBRETTO_IA_OGNI_S = 2.0;
constexpr double VISTA_RECENTE_S = 3.0;         // il contatto visto adesso
constexpr double RIVEDI_S = 300.0;              // dopo, una nuova identificazione sullo stesso contatto
constexpr double RIVEDI_PIU_VICINO = 0.6;       // o prima, se l'identificazione sbagliata era da molto piu' lontano
constexpr double PORTATA_MIN_M = 1000.0;          // la portata su cui si misura la P di sbagliare non scende sotto
constexpr double NOTTE = 0.2;                   // luce sotto cui e' notte (come l'artiglieria)
constexpr double P_BASE = 0.03, P_DISTANZA = 0.6, P_NOTTE = 0.15, P_PRUA = 0.15, P_VELA = 0.10, P_MAX = 0.85;
constexpr double PRUA_GRADI = 25.0;             // entro questo angolo dalla prua o dalla poppa la sagoma si confonde
constexpr double VISIVA_VALE_S = 900.0;         // per lato_creduto
// l'AOB dell'IA al periscopio (simmetria, docs/25 §3.5): la rotta stimata della nave vista, nella TMA del battello
constexpr double AOB_IA_OGNI_S = 60.0;          // un'osservazione al minuto per bersaglio

bool arma(const UnitaClassica& u) { return (u.flag & 0x20) != 0 || u.categoria == "arma"; }

}  // namespace

const Missione::IdentificazioneVisiva* Missione::identificazione_visiva(int osservatore, int contatto) const {
    const auto it = ultima_identificazione_.find({osservatore, contatto});
    return it == ultima_identificazione_.end() ? nullptr : &identificazioni_visive_[it->second];
}

std::optional<int> Missione::lato_visivo(int osservatore, int contatto) const {
    const auto* v = identificazione_visiva(osservatore, contatto);
    if (!v || tempo_ - v->t > VISIVA_VALE_S) return std::nullopt;
    return parte_della_classe(v->classe);
}

// la parte di una classe come la sa chi la identifica: quella di un'unita' della missione di quella classe, le civili
// neutrali; nullopt se la classe non c'e' (il giocatore puo' dichiarare una classe che non e' in mare)
std::optional<int> Missione::parte_della_classe(const std::string& classe) const {
    for (const auto& [id, u] : unita_)
        if (u.classe == classe && !arma(u)) return u.flag & 0x18;
    if (classe_civile(classe)) return 0;
    return std::nullopt;
}

void Missione::nota_identificazione(int osservatore, int contatto, const std::string& classe, bool giusta, double distanza_m) {
    identificazioni_visive_.push_back({tempo_, osservatore, contatto, classe, giusta, distanza_m});
    ultima_identificazione_[{osservatore, contatto}] = identificazioni_visive_.size() - 1;
}

bool Missione::ordine_libretto(const Ordine& o) {
    auto* g = giocatore();
    if (!peri_ || !g) return false;
    const auto k = g->contatti.find(o.n);
    const auto* b = trova(o.n);
    if (k == g->contatti.end() || !b) return false;
    if (o.tipo == TipoOrdine::identifica_visiva) {
        if (o.testo.empty()) return false;   // la chiave della classe (classi.toml), come classifica_contatto
        nota_identificazione(g->id, b->id, o.testo, o.testo == b->classe, distanza(g->pos, b->pos));
        registro_.classifica(*this, g->id, b->id, o.testo, true);
        // achievement e statistiche della notte: giusta coi soli fanali (oltre la sagoma, dalla testa del periscopio) o con la
        // sagoma; a luci spente, giusta o errata
        if (luci_attive_ && luce_periscopi() < NOTTE) {
            const bool giusta = o.testo == b->classe, spente = stato_luci(b->id) == luci::Stato::spente;
            Osservatore occhio;
            occhio.altezza_occhio_m = 1.0;
            occhio.allertato = true;
            if (giusta) {
                if (distanza(g->pos, b->pos) > std::max(PORTATA_MIN_M, portata_sagoma(b->id, g->pos, occhio))) ++conti_.visive_fanali_giuste;
                else ++conti_.visive_notte_sagoma;
                if (spente) ++conti_.visive_luci_spente;
            } else if (spente) {
                ++conti_.errate_luci_spente;
            }
        }
        return true;
    }
    if (o.tipo == TipoOrdine::osserva_aob) {
        if (std::fabs(o.a) > 180.0 || o.b < 0.0) return false;
        OsservazioneAob x;
        x.t = tempo_;
        x.contatto = b->id;
        x.aob_gradi = o.a;
        x.incertezza_gradi = o.b;
        // col periscopio alzato il rilevamento e' quello preso a vista adesso (la linea di mira sul bersaglio); se no
        // quello del contatto, che puo' essere un sonar vecchio
        x.rilevamento = testa_giocatore() ? normalizza_360(rilevamento(g->pos, b->pos)) : k->second.rilevamento;
        x.rotta = normalizza_360(x.rilevamento + 180.0 - o.a);
        x.rotta_vera = normalizza_360(b->rotta);
        osservazioni_aob_.push_back(x);
        // alla TMA dell'equipaggio (il piano dei missili, i contatti noti): la rotta col suo sigma (nucleo/tma.hpp)
        if (catalogo_missili_) tma_equipaggio_[b->id].tma.aggiungi_rotta({tempo_, x.rotta, std::max(o.b, 1.0)});
        return true;
    }
    if (o.tipo == TipoOrdine::misura_telemetro) {   // la lettura e l'altezza del quadrante; la vera solo per il registro
        if (o.a <= 0.0 || o.b <= 0.0) return false;
        misure_telemetro_.push_back({tempo_, b->id, o.a, distanza(g->pos, b->pos), o.b});
        return true;
    }
    return false;
}

void Missione::stima_tma_giocatore(int contatto, double rotta) {
    const UnitaClassica* b = trova(contatto);
    if (!b) return;
    StimaRottaTma x;
    x.t = tempo_;
    x.contatto = contatto;
    x.rotta = normalizza_360(rotta);
    x.rotta_vera = normalizza_360(b->rotta);
    stime_tma_giocatore_.push_back(x);
}

void Missione::passo_libretto_ia() {
    if (tempo_ < prossime_identificazioni_ia_) return;
    prossime_identificazioni_ia_ = tempo_ + LIBRETTO_IA_OGNI_S;
    if (!caso_libretto_) caso_libretto_.emplace(seme_ ^ 0x11B2E770ull);
    const bool notte = luce_periscopi() < NOTTE;
    for (const auto& [id, o] : unita_) {
        if (o.giocatore || o.distrutta || o.persa || arma(o)) continue;
        // chi guarda: navi e aerei sempre; i sottomarini col periscopio fuori o emersi
        const bool guarda = o.categoria != "sottomarino" || o.quota < 0.5 || testa_ia_fuori(id);
        // l'occhio di chi guarda: le vedette sul ponte, l'aereo alla sua quota, la testa del periscopio
        Osservatore occhio;
        occhio.altezza_occhio_m = o.categoria == "nave" ? 20.0 : aereo(o) ? quota_aerea(o) : 1.0;
        occhio.allertato = true;
        for (const auto& [cid, c] : o.contatti) {
            if (tempo_ - c.ultima_scoperta > VISTA_RECENTE_S) continue;
            const auto* b = trova(cid);
            if (!b || b->distrutta || b->persa || arma(*b) || (b->categoria != "nave" && b->categoria != "sottomarino")) continue;
            const double d = distanza(o.pos, b->pos);
            // la portata a vista della sua sagoma a ogni luce, di notte anche dei fanali (portata_vista, missione_luci.cpp): la
            // vela di un sottomarino emerso, piu' bassa, ha la sua sagoma
            const double portata = std::max(PORTATA_MIN_M, portata_vista(cid, o.pos, occhio));
            // la vista, o il radar entro la portata della vista: la vedetta guarda dove dice il radar
            if (c.sensore != "vista" && !(c.sensore == "radar" && guarda && d <= portata && (b->categoria == "nave" || b->quota <= 3.0))) continue;
            // di notte oltre la sagoma, dentro i soli fanali: la classe no, quello che dicono i fanali si' (leggi_fanali)
            if (notte && luci_attive_ && d > std::max(PORTATA_MIN_M, portata_sagoma(cid, o.pos, occhio))) {
                const LetturaFanali l = leggi_fanali(cid, o.pos, occhio);
                if (l.visti > 0) fanali_ia_[{id, cid}] = {tempo_, l};
                continue;
            }
            if (const auto* v = identificazione_visiva(id, cid))
                if (tempo_ - v->t < RIVEDI_S && (v->giusta || d > RIVEDI_PIU_VICINO * v->distanza_m)) continue;
            // la P di sbagliare: la distanza sulla portata della vista (la foschia la accorcia), la notte, la prua
            const double r = d / portata;
            double p = P_BASE + P_DISTANZA * r * r;
            if (notte) p += P_NOTTE;
            const double aspetto = std::fabs(normalizza_180(rilevamento(b->pos, o.pos) - b->rotta));
            if (aspetto < PRUA_GRADI || aspetto > 180.0 - PRUA_GRADI) p += P_PRUA;
            if (b->categoria == "sottomarino") p += P_VELA;
            p = std::min(p, P_MAX);
            std::string classe = b->classe;
            if (caso_libretto_->uniforme() < p) {
                // l'errore: un'altra classe dello stesso genere fra quelle in mare (una fregata per un mercantile)
                std::set<std::string> altre;
                for (const auto& [xid, x] : unita_)
                    if (!arma(x) && x.categoria == b->categoria && x.classe != b->classe && !x.classe.empty()) altre.insert(x.classe);
                if (!altre.empty()) {
                    auto it = altre.begin();
                    std::advance(it, static_cast<long>(caso_libretto_->uniforme() * static_cast<double>(altre.size())) % static_cast<long>(altre.size()));
                    classe = *it;
                }
            }
            // un civile (flag 40h o classe civile) non ha il libretto: vede una nave o un sottomarino, non la classe; la
            // vista dice la natura, NONSUB per una nave e CERTSUB per un sottomarino (disassemblatore, 30/10/2026: nella S15
            // l'Ugra dava CERTSUB a una Krivak, nella S08 il cargo diceva Los Angeles un Foxtrot). Le estrazioni restano le
            // stesse anche per il civile, per non spostare quelle degli altri
            const bool civile = (o.flag & 0x40) || classe_civile(o.classe);
            if (civile) classe.clear();
            const bool giusta = civile || classe == b->classe;
            nota_identificazione(id, cid, classe, giusta, d);
            const Confidenza natura = b->categoria == "sottomarino" ? Confidenza::certsub : Confidenza::nonsub;
            classificazioni_ia_.push_back({tempo_, id, cid, classe, natura, !giusta, true});
        }
        // l'AOB al periscopio, per la simmetria col giocatore (osserva_aob; nucleo/aob_ia): un sottomarino col periscopio
        // fuori stima l'AOB delle navi e dei sottomarini emersi che vede, come il giocatore (passi di 10 gradi, la sigma
        // che cresce con la distanza sulla portata, la notte coi fanali o sulla sola sagoma, la sola vela, il lato
        // ambiguo vicino all'asse), e mette la rotta che ne viene nella sua TMA
        if (o.categoria != "sottomarino" || !testa_ia_fuori(id)) continue;
        for (const auto& [cid, c] : o.contatti) {
            const auto* b = trova(cid);
            if (!b || b->distrutta || b->persa) continue;
            const bool vela = b->categoria == "sottomarino";
            if (!(b->categoria == "nave" || (vela && b->quota <= 1.0))) continue;
            // la portata come per l'identificazione: la sagoma a ogni luce (la vela ha la sua), di notte anche i fanali
            const double portata = std::max(PORTATA_MIN_M, portata_vista(cid, o.pos, occhio));
            const double d = distanza(o.pos, b->pos);
            if (d > portata) continue;   // il sensore del contatto lo riscrive il passivo: conta la vista dal periscopio
            const auto t = tma_ia_.find({id, cid});
            if (t == tma_ia_.end()) continue;   // la TMA del battello su quel contatto (missione.cpp, i passivi)
            const auto& rotte = t->second.tma.rotte();
            if (!rotte.empty() && tempo_ - rotte.back().tempo < AOB_IA_OGNI_S) continue;
            CondizioniAob ca;
            ca.distanza_m = d;
            ca.portata_m = portata;
            ca.luce = luce_periscopi();
            ca.solo_vela = vela;
            ca.luci = notte && luci_attive_ && stato_luci(cid) != luci::Stato::spente;
            const double ril = rilevamento(o.pos, b->pos);
            const StimaAob x = stima_aob(aob_vero(ril, b->rotta), ril, ca, *caso_libretto_);
            t->second.tma.aggiungi_rotta({tempo_, x.rotta, x.sigma_gradi});
        }
    }
}

}  // namespace m688
