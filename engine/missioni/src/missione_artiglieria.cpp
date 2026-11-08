#include <algorithm>
#include <cmath>
#include <string>

#include "missioni/artiglieria_missione.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "missioni/periscopi_missione.hpp"
#include "nucleo/acustica_artiglieria.hpp"
#include "nucleo/carriera.hpp"

extern "C" {
#include "lua.h"
}

// Artiglieria navale contro il sottomarino nella missione (docs/16_artiglieria.md, G2; dossier 22 §9.2, citato «22»):
// il primo pezzo di dottrina delle navi. Ogni nave armata ostile al giocatore spara quando lo vede esposto (in
// superficie, una testa o un albero) da radar o vedetta con traccia solida, se la situazione e la portata lo
// consentono. Il tiro e gli impatti sono del modulo puro nucleo/artiglieria. Solo con la regola di simulazione.

namespace m688 {

namespace {

constexpr double VISTO_RECENTE_S = 10.0;     // una scoperta radar o a vista piu' vecchia non tiene il bersaglio
constexpr double CESSATE_S = 5.0;            // il fuoco cessa 5 s dopo la sparizione del bersaglio (22 §9.2)
constexpr double PERSO_S = 120.0;            // poi la designazione si perde: un nuovo bersaglio rifa' la reazione
constexpr double FUOCO_AMICO_GRADI = 15.0;   // nessun colpo con una nave amica vicino alla linea (22 §9.2)
constexpr double FUOCO_AMICO_OLTRE_M = 1000.0;
constexpr double NOTTE = 0.2;                // luce sotto cui e' notte (periscopi: 0 notte, 1 giorno)
constexpr int AVVERTIMENTI_PER_SALVA = 3;    // crisi: tre colpi d'avvertimento, poi una pausa (STIMA)
constexpr double PAUSA_AVVERTIMENTI_S = 120.0;
constexpr double ILLUMINANTE_OGNI_S = 50.0;  // un illuminante dura ~50 s (22 §6.3, §10)
constexpr double ILLUMINANTE_QUOTA_M = 450.0, ILLUMINANTE_SPARSO_M = 300.0;   // STIMA
// i primi colpi di un pezzo su un bersaglio nuovo hanno l'errore del «primo colpo» (22 §3.6): manca la correzione sul
// punto di caduta. Tre (STIMA, taratura G7.1: periscopio a 3 km da una Perry in allarme colpito circa una volta su due)
constexpr int COLPI_AGGIUSTAMENTO = 3;
constexpr double ALLARME_A_COMBATTIMENTO_S = 120.0;   // dal primo contatto al posto di combattimento (STIMA, 22 §4.2)
// la reazione fino al primo colpo: 0,9-1,7 volte quella del pezzo (22 §4.2: 15-40 s al posto di combattimento; tarata
// con G7.1: 0,58 su 200 semi)
constexpr double REAZIONE_MIN = 0.9, REAZIONE_MAX = 1.7;
constexpr double CALIBRO_ILLUMINANTE_MM = 100.0;   // i pezzi da 100 mm in su hanno l'illuminante (STIMA)
// id 995000 + 10 n (acustica_artiglieria.hpp); dal 3/11/2026 la fascia 990000-994999 e' dei falsi contatti (D3). Il seme
// dei transitori resta quello di prima (990000 + 10 n, n fino a 999), cosi' i suoni non cambiano
constexpr int TRANSITORI_DA = 995000, TRANSITORI_QUANTI = 500, SEME_DA = 990000, SEMI_QUANTI = 1000;
constexpr double ALBERO_DIAMETRO_M = 0.2, ANTENNA_M = 1.2, ANTENNA_DIAMETRO_M = 0.15;   // STIMA
constexpr double PERISCOPIO_CLASSICO_M = 1.5;      // la testa senza i periscopi dell'Enhanced
constexpr double VELA_AFFIORATA_M = 0.8;           // la cima della vela col mare grosso (STIMA)
// le navi bersaglio (fra_navi): bordo libero e sovrastrutture (STIMA)
constexpr double BORDO_NAVE_MIN_M = 3.0, BORDO_PER_PESCAGGIO = 0.8;
constexpr double SOVRASTRUTTURE_LUNGHEZZA = 0.4, SOVRASTRUTTURE_LARGHEZZA = 0.8, SOVRASTRUTTURE_ALTEZZA_M = 8.0;

// esperto (22 §9.9): il fuoco sulla piuma della testa alzata, senza la testa vista, entro 1 km (di notte, la piuma
// fosforescente, 300 m: 22 §4.1); la piuma dal fattore del periscopio e dalla velocita' (nucleo/periscopi), l'errore
// della stima sul punto (STIMA)
constexpr double PIUMA_MIN = 0.2, PIUMA_GIORNO_M = 1000.0, PIUMA_NOTTE_M = 300.0, PIUMA_ERRORE_M = 25.0;
constexpr int PIUMA_MARE_MAX = 4;   // oltre, i frangenti la nascondono (22 §4.1: «molto meno con mare >= 4»)
// acque territoriali in pace (22 §2.4 e §9.2): bombe d'avvertimento al battello immerso con la posizione nota
constexpr double BOMBE_PORTATA_M = 3000.0, BOMBE_OGNI_S = 120.0, BOMBE_SCENDONO_S = 8.0, BOMBA_KG = 1.0;
constexpr int BOMBE_PER_VOLTA = 2;
constexpr double BOMBE_LONTANE_M = 500.0, BOMBE_LONTANE_FINO_M = 1000.0;   // poi, dopo BOMBE_VICINE_DOPO_S dentro,
constexpr double BOMBE_VICINE_M = 200.0, BOMBE_VICINE_FINO_M = 400.0, BOMBE_VICINE_DOPO_S = 1200.0;
// un sottomarino dell'IA sotto il tiro si immerge (22 §9.8): alberi giu' e almeno a questa quota (STIMA)
constexpr double TIRO_VICINO_M = 300.0, QUOTA_SICURA_IA_M = 60.0;
constexpr int MSG_ACQUE_ENTRATA = 453, MSG_ACQUE_USCITA = 454, MSG_BOMBE_AVVERTIMENTO = 455;

double fattore_reazione(std::optional<DifficoltaAsw> d) {   // 22 §9.9
    if (!d) return 1.0;
    if (*d == DifficoltaAsw::facile) return 2.0;
    if (*d == DifficoltaAsw::minaccia_percepita) return 0.7;
    return 1.0;
}

}  // namespace

void Missione::artiglieria_enhanced(const std::string& cartella_dati) {
    if (artiglieria_ || artiglieria_spenta_) return;
    auto s = std::make_unique<StatoArtiglieriaMissione>();
    try {
        s->dati = carica_artiglieria(cartella_dati + "/artiglieria.toml");
    } catch (const std::exception& e) {
        errori_.push_back(e.what());
        artiglieria_spenta_ = true;
        return;
    }
    if (s->dati.vuota()) {   // senza i dati: nessuna artiglieria
        artiglieria_spenta_ = true;
        return;
    }
    // la situazione: il blocco carriera della missione, o quella delle originali in carriera.toml
    if (blocco_carriera_) {
        s->situazione = situazione_da_nome(blocco_carriera_->situazione).value_or(Situazione::pace);
    } else {
        try {
            const RegoleCarriera r = carica_carriera(cartella_dati + "/carriera.toml");
            if (const auto it = r.originali.find(id_); it != r.originali.end()) s->situazione = it->second.situazione;
        } catch (const std::exception&) {}
    }
    // la chiave `artiglieria = { fra_navi = true }` della missione: in guerra le navi sparano anche alle navi ostili
    lua_rawgeti(L_, LUA_REGISTRYINDEX, rif_missione_);
    lua_getfield(L_, -1, "artiglieria");
    if (lua_istable(L_, -1)) {
        lua_getfield(L_, -1, "fra_navi");
        s->fra_navi = lua_toboolean(L_, -1);
        lua_pop(L_, 1);
    }
    lua_pop(L_, 2);
    s->caso = Casuale(seme_ ^ 0xA27151E7A27151E7ull);
    carica_dimensioni_classi(cartella_dati);
    artiglieria_ = std::move(s);
}

bool Missione::artiglieria_avvertimento() const {
    if (artiglieria_ && forze_)
        if (const auto* g = giocatore())
            if (const auto s = situazione_nota(g->flag & 0x18, (g->flag & 0x18) == 0x08 ? 0x10 : 0x08)) return *s == Situazione::crisi;
    return artiglieria_ && artiglieria_->situazione == Situazione::crisi && !artiglieria_->provocati;
}

SagomaEsposta Missione::sagoma_esposta(const UnitaClassica& u) const {
    std::vector<AlberoAlzato> alberi;
    if (u.giocatore && peri_) {
        const StatoPeriscopio& st = peri_->posto.stato();
        if (st.testa_fuori && st.altezza_vera_m > 0.0) {
            const DatiPeriscopio* d = peri_->posto.dati();
            const ParteEsposta parte = d && d->ruolo == "ricerca" ? ParteEsposta::periscopio_ricerca : ParteEsposta::periscopio_attacco;
            alberi.push_back({parte, st.altezza_vera_m, std::max(ALBERO_DIAMETRO_M, d ? d->testa_m : 0.0)});
        }
    } else if ((u.flag54 & 2) && u.quota <= QUOTA_PERISCOPICA_MISSIONE_M + 1.0) {
        alberi.push_back({ParteEsposta::periscopio_attacco, PERISCOPIO_CLASSICO_M - std::max(0.0, u.quota - QUOTA_PERISCOPICA_MISSIONE_M), ALBERO_DIAMETRO_M});
    } else if (!u.giocatore && testa_ia_fuori(u.id)) {
        alberi.push_back({ParteEsposta::periscopio_attacco, PERISCOPIO_CLASSICO_M, ALBERO_DIAMETRO_M});
    }
    if ((u.flag54 & 4) && u.quota <= QUOTA_PERISCOPICA_MISSIONE_M + 1.0)
        alberi.push_back({ParteEsposta::albero_radio, ANTENNA_M - std::max(0.0, u.quota - QUOTA_PERISCOPICA_MISSIONE_M), ANTENNA_DIAMETRO_M});
    double lunghezza = 100.0, larghezza = 10.0;
    if (const auto d = dimensioni_classi_.find(u.classe); d != dimensioni_classi_.end() && d->second.lunghezza_m > 0.0) {
        lunghezza = d->second.lunghezza_m;
        larghezza = d->second.larghezza_m;
    }
    const SagomaClasse c = artiglieria_ ? artiglieria_->dati.sagoma(u.classe) : SagomaClasse{};
    const double vela = u.giocatore && vela_affiorata() ? VELA_AFFIORATA_M : 0.0;
    return sagoma_sottomarino(c, u.id, u.pos, u.rotta, u.velocita * m_s_per_nodo_, lunghezza, larghezza, u.quota, vela, alberi);
}

SagomaEsposta Missione::sagoma_nave(const UnitaClassica& u) const {
    // una nave bersaglio (fra_navi): lo scafo col bordo libero e le sovrastrutture (STIMA)
    SagomaEsposta g;
    g.unita = u.id;
    g.pos = u.pos;
    g.rotta_deg = u.rotta;
    g.velocita_m_s = u.velocita * m_s_per_nodo_;
    g.in_superficie = true;
    g.lunghezza_m = 100.0;
    g.larghezza_m = 12.0;
    if (const auto d = dimensioni_classi_.find(u.classe); d != dimensioni_classi_.end() && d->second.lunghezza_m > 0.0) {
        g.lunghezza_m = d->second.lunghezza_m;
        g.larghezza_m = d->second.larghezza_m;
    }
    g.bordo_libero_m = std::max(BORDO_NAVE_MIN_M, BORDO_PER_PESCAGGIO * pescaggio(u));
    g.falsatorre_lunghezza_m = SOVRASTRUTTURE_LUNGHEZZA * g.lunghezza_m;
    g.falsatorre_larghezza_m = SOVRASTRUTTURE_LARGHEZZA * g.larghezza_m;
    g.falsatorre_altezza_m = g.bordo_libero_m + SOVRASTRUTTURE_ALTEZZA_M;
    return g;
}

// In pace nelle acque del paese costiero (22 §2.4 e §9.2): col giocatore immerso e la posizione nota, due bombe
// d'avvertimento ogni 2 minuti lontano dalla stima, piu' vicino dopo 20 minuti dentro. Vero se la nave ha fatto la sua
// parte cosi' (il battello non e' esposto): allora niente cannone; falso se il giocatore e' esposto (i colpi
// d'avvertimento della crisi)
bool Missione::bombe_avvertimento(const UnitaClassica& nave, const UnitaClassica& g) {
    auto& S = *artiglieria_;
    auto& n = S.navi.at(nave.id);
    if (!sagoma_esposta(g).vuota()) return false;
    const auto ci = nave.contatti.find(g.id);
    if (ci == nave.contatti.end() || !ci->second.posizione_nota || ci->second.qualita < 800.0) return true;
    if (distanza(nave.pos, ci->second.pos_stimata) > BOMBE_PORTATA_M || tempo_ < n.prossima_bomba) return true;
    n.prossima_bomba = tempo_ + BOMBE_OGNI_S;
    const bool vicine = tempo_ - S.acque_da >= BOMBE_VICINE_DOPO_S;
    for (int k = 0; k < BOMBE_PER_VOLTA; ++k) {
        ImpattoArtiglieria b;
        b.tiratore = nave.id;
        b.bersaglio = g.id;
        b.pezzo = "bomba_avvertimento";
        b.carica_kg = BOMBA_KG;
        b.tempo_sparo_s = tempo_;
        b.tempo_impatto_s = tempo_ + BOMBE_SCENDONO_S + k;
        const double r = vicine ? S.caso.uniforme(BOMBE_VICINE_M, BOMBE_VICINE_FINO_M) : S.caso.uniforme(BOMBE_LONTANE_M, BOMBE_LONTANE_FINO_M);
        const double a = S.caso.uniforme(0.0, 2.0 * 3.14159265358979323846);
        b.punto = {ci->second.pos_stimata.x + r * std::sin(a), ci->second.pos_stimata.y + r * std::cos(a)};
        b.avvertimento = true;
        for (auto e : eventi_di(b, nave.pos, false)) S.eventi_in_volo.push_back(e);
        S.in_volo.push_back(b);
    }
    return true;
}

int Missione::acque_territoriali_di(const UnitaClassica& u) const {
    const int lato = u.flag & 0x18;
    for (std::size_t i = 0; i < acque_territoriali_.size(); ++i) {
        const auto& a = acque_territoriali_[i];
        const bool suo = a.lato == 0x18 ? (lato == 0 || lato == 0x18) : lato == a.lato;
        if (!suo && dentro_zona(a.zona, u.pos)) return static_cast<int>(i);
    }
    return -1;
}

bool Missione::vede_la_piuma(const UnitaClassica& nave) const {
    const auto* g = giocatore();
    if (!g || g->distrutta || g->persa || !difficolta_asw_ || *difficolta_asw_ != DifficoltaAsw::minaccia_percepita) return false;
    if (nave.categoria != "nave" || !ostili(nave, *g) || stato_mare() > PIUMA_MARE_MAX || piuma_giocatore() < PIUMA_MIN) return false;
    const SagomaEsposta s = sagoma_esposta(*g);
    if (s.vuota() || s.in_superficie) return false;
    const bool notte = luce_periscopi() < NOTTE;
    return distanza(nave.pos, g->pos) <= (notte ? PIUMA_NOTTE_M : PIUMA_GIORNO_M);
}

double Missione::piuma_giocatore() const {
    const auto* g = giocatore();
    if (!g) return 0.0;
    if (peri_) {
        const StatoPeriscopio& st = peri_->posto.stato();
        const DatiPeriscopio* d = peri_->posto.dati();
        return st.testa_fuori && d ? piuma(*d, g->velocita) : 0.0;
    }
    // senza i periscopi dell'Enhanced: la testa del classico col fattore d'attacco (0,5)
    return (g->flag54 & 2) ? 0.5 * std::clamp((std::abs(g->velocita) - 2.0) / 8.0, 0.0, 1.5) : 0.0;
}

void Missione::posto_di_combattimento(int nave) {
    if (!artiglieria_) return;
    if (const auto it = unita_.find(nave); it == unita_.end() || it->second.categoria != "nave") return;
    auto& S = *artiglieria_;
    auto it = S.navi.find(nave);
    if (it == S.navi.end()) {
        StatoArtiglieriaMissione::Nave n;
        for (const DatiPezzo* p : S.dati.pezzi_di(unita_.at(nave).classe)) n.pezzi.push_back({p, p->colpi_pronti, 0.0, -1.0, 0});
        it = S.navi.emplace(nave, std::move(n)).first;
    }
    it->second.combattimento = true;
}

void Missione::passo_artiglieria() {
    auto& S = *artiglieria_;
    const UnitaClassica* g = giocatore();
    // una crisi diventa guerra se il giocatore lancia (autodifesa delle navi)
    const auto& ev = registro_.eventi();
    for (; S.lanci_visti < ev.size(); ++S.lanci_visti)
        if (ev[S.lanci_visti].tipo == TipoEvento::lancio_giocatore) S.provocati = true;
    // con le forze dell'Enhanced (fase 3) la situazione e' quella fra la parte della nave e l'altra, provocazioni comprese
    const Situazione propria = S.provocati && S.situazione == Situazione::crisi ? Situazione::guerra : S.situazione;
    const auto nota = [&](int a, int b) { return forze_ ? situazione_nota(a, b).value_or(propria) : propria; };
    const Situazione sit = forze_ ? std::max({nota(0x08, 0x10), nota(0, 0x08), nota(0, 0x10)}) : propria;
    // le acque territoriali straniere dove sta il giocatore: entrata e uscita (registro, messaggi)
    const int acque_g = g && !g->distrutta && !g->persa ? acque_territoriali_di(*g) : -1;
    if (acque_g != S.acque_dentro) {
        if (S.acque_dentro >= 0) passaggi_acque_.push_back({tempo_, S.acque_dentro, false});
        if (acque_g >= 0) passaggi_acque_.push_back({tempo_, acque_g, true});
        if (g) messaggio_di_bordo(g->id, acque_g >= 0 ? MSG_ACQUE_ENTRATA : MSG_ACQUE_USCITA);
        S.acque_dentro = acque_g;
        S.acque_da = tempo_;
    }
    if (sit != Situazione::pace || acque_g >= 0 || !regolazioni_.empty()) {   // la sandbox: le ROE possono liberare i pezzi
        const bool notte = luce_periscopi() < NOTTE;
        const bool principiante = difficolta_asw_ && *difficolta_asw_ == DifficoltaAsw::facile;
        // i bersagli possibili: il giocatore (il sottomarino esposto) e, con fra_navi in guerra, le navi
        struct Bersaglio {
            const UnitaClassica* u = nullptr;
            SagomaEsposta vera;
            Vec2 mira, vel;
            bool esposta = false;
        };
        const auto bersaglio_di = [&](const UnitaClassica& t) {
            Bersaglio b;
            b.u = &t;
            b.vera = t.categoria == "nave" ? sagoma_nave(t) : sagoma_esposta(t);
            b.esposta = !b.vera.vuota();
            b.mira = punto_di_mira(b.vera);
            const double r = radianti(t.rotta), v = t.velocita * m_s_per_nodo_;
            b.vel = {std::sin(r) * v, std::cos(r) * v};
            return b;
        };
        std::optional<Bersaglio> del_giocatore;
        if (g && !g->distrutta && !g->persa && g->affondamento == 0) del_giocatore = bersaglio_di(*g);   // non su chi affonda
        for (const auto& [id, u] : unita_) {
            if (u.categoria != "nave" || u.distrutta || u.persa || u.giocatore) continue;
            auto it = S.navi.find(id);
            if (it == S.navi.end()) {
                StatoArtiglieriaMissione::Nave n;
                for (const DatiPezzo* p : S.dati.pezzi_di(u.classe)) n.pezzi.push_back({p, p->colpi_pronti, 0.0, -1.0, 0});
                it = S.navi.emplace(id, std::move(n)).first;
            }
            auto& n = it->second;
            // in pace, solo le navi del paese costiero e solo contro il giocatore nelle sue acque, come in crisi
            const int lato_nave = u.flag & 0x18;
            const bool costiera = sit == Situazione::pace && acque_g >= 0 &&
                                  (acque_territoriali_[static_cast<std::size_t>(acque_g)].lato == 0x18 ? (lato_nave == 0 || lato_nave == 0x18)
                                                                                                      : lato_nave == acque_territoriali_[static_cast<std::size_t>(acque_g)].lato);
            const int opposto = lato_nave == 0x08 ? 0x10 : lato_nave == 0x10 ? 0x08 : g ? (g->flag & 0x18) : 0x08;
            const Situazione sn = costiera ? Situazione::crisi : forze_ ? nota(lato_nave, opposto) : sit;
            // la sandbox: le ROE per categoria della parte della nave vengono prima della situazione (libere: come in
            // guerra; trattenute: niente tiro, nemmeno d'avvertimento), per i sottomarini e per le navi
            const auto con_roe = [&](CategoriaRoe c) {
                const auto r = roe_parte(lato_nave, c);
                return r ? (*r ? Situazione::guerra : Situazione::pace) : sn;
            };
            const Situazione sn_sub = con_roe(CategoriaRoe::sottomarini), sn_navi = con_roe(CategoriaRoe::navi);
            if (sn_sub == Situazione::pace && sn_navi == Situazione::pace) continue;
            if (costiera && bombe_avvertimento(u, *g)) continue;
            if (n.pezzi.empty()) continue;
            // il bersaglio: il giocatore esposto e visto da radar o vedetta; se no (fra_navi, in guerra) la nave ostile
            // piu' vicina con un contatto solido e la posizione
            std::optional<Bersaglio> scelto;
            const Contatto* contatto = nullptr;
            Contatto sulla_piuma;   // esperto: il punto stimato della piuma, senza contatto
            const auto visto_ora = [&](const Contatto& c) {
                return c.qualita >= 800.0 && (c.sensore == "radar" || c.sensore == "vista") && tempo_ - c.ultima_scoperta <= VISTO_RECENTE_S;
            };
            if (del_giocatore && (ostili(u, *g) || costiera) && sn_sub != Situazione::pace) {
                const auto ci = u.contatti.find(g->id);
                if (ci != u.contatti.end() && sn_sub == Situazione::guerra && n.allarme_da < 0.0) n.allarme_da = tempo_;   // allarme
                if (del_giocatore->esposta && ci != u.contatti.end() && visto_ora(ci->second)) {
                    scelto = del_giocatore;
                    contatto = &ci->second;
                }
                // esperto (22 §9.9): la piuma della testa alzata basta, entro 1 km di giorno e 300 m di notte
                if (!scelto && sn_sub == Situazione::guerra && vede_la_piuma(u)) {
                    sulla_piuma.pos_stimata = {g->pos.x + PIUMA_ERRORE_M * S.caso.uniforme(-1.0, 1.0),
                                               g->pos.y + PIUMA_ERRORE_M * S.caso.uniforme(-1.0, 1.0)};
                    scelto = del_giocatore;
                    contatto = &sulla_piuma;
                    if (n.allarme_da < 0.0) n.allarme_da = tempo_;
                }
            }
            // in guerra anche i sottomarini dell'IA esposti e visti, il piu' vicino
            if (!scelto && sn_sub == Situazione::guerra) {
                double vicina = 1e18;
                for (const auto& [tid, t] : unita_) {
                    if (t.categoria != "sottomarino" || t.giocatore || t.distrutta || t.persa || t.affondamento > 0 || (t.flag & 0x20) || !ostili(u, t)) continue;
                    const auto ci = u.contatti.find(tid);
                    if (ci == u.contatti.end() || !visto_ora(ci->second)) continue;
                    const double d = distanza(u.pos, t.pos);
                    if (d >= vicina) continue;
                    Bersaglio b = bersaglio_di(t);
                    if (!b.esposta) continue;
                    vicina = d;
                    scelto = b;
                    contatto = &ci->second;
                }
                if (scelto && n.allarme_da < 0.0) n.allarme_da = tempo_;
            }
            if (!scelto && S.fra_navi && sn_navi == Situazione::guerra) {
                double vicina = 1e18;
                for (const auto& [tid, t] : unita_) {
                    if (t.categoria != "nave" || t.distrutta || t.persa || t.affondamento > 0 || !ostili(u, t)) continue;
                    const auto ci = u.contatti.find(tid);
                    if (ci == u.contatti.end() || ci->second.qualita < 800.0 || !ci->second.posizione_nota) continue;
                    if (const double d = distanza(u.pos, t.pos); d < vicina) {
                        vicina = d;
                        scelto = bersaglio_di(t);
                        contatto = &ci->second;
                    }
                }
                if (scelto && n.allarme_da < 0.0) n.allarme_da = tempo_;
            }
            // l'allarme porta la nave al posto di combattimento dopo la reazione di condizione III (22 §4.2: 1-3 min)
            if (!n.combattimento && n.allarme_da >= 0.0 && tempo_ - n.allarme_da >= ALLARME_A_COMBATTIMENTO_S) n.combattimento = true;
            if (scelto && scelto->u->id != n.bersaglio) { n.acquisito = false; n.bersaglio = scelto->u->id; }
            const bool visto = scelto.has_value();
            if (visto) {
                if (!n.acquisito) {   // la designazione: la reazione fino al primo colpo, per pezzo (22 §4.2)
                    n.acquisito = true;
                    for (auto& p : n.pezzi) {
                        const double base = n.combattimento ? p.dati->reazione_combattimento_s : p.dati->reazione_condizione3_s;
                        p.prossimo = std::max(p.prossimo, tempo_ + base * S.caso.uniforme(REAZIONE_MIN, REAZIONE_MAX) * fattore_reazione(difficolta_asw_));
                        p.sparati = 0;
                    }
                }
                n.ultimo_visto = tempo_;
                n.punto = {contatto->pos_stimata.x + scelto->mira.x - scelto->vera.pos.x, contatto->pos_stimata.y + scelto->mira.y - scelto->vera.pos.y};
                n.punto_finale = false;
            } else if (n.acquisito) {
                if (tempo_ - n.ultimo_visto > PERSO_S) n.acquisito = false;
                if (tempo_ - n.ultimo_visto <= CESSATE_S) {
                    // il bersaglio e' appena sparito: si continua fino al cessate il fuoco
                } else if (!n.punto_finale) {
                    n.punto_finale = true;   // l'ultimo colpo sul punto, dopo il cessate il fuoco
                } else {
                    continue;
                }
            } else {
                continue;
            }
            // il bersaglio (anche sparito: la sua sagoma di adesso, vuota se e' sotto)
            const auto bi = unita_.find(n.bersaglio);
            if (bi == unita_.end() || bi->second.distrutta || bi->second.persa) { n.acquisito = false; continue; }
            const UnitaClassica* bt = &bi->second;
            const Bersaglio B = scelto ? *scelto : bersaglio_di(*bt);
            const SagomaEsposta& vera = B.vera;
            const Vec2 vel = B.vel;
            // illuminanti di notte (anche in crisi): sopra il bersaglio, uno ogni ~50 s per nave, da un pezzo da 100 mm in
            // su gia' pronto (la reazione del §4.2): la partenza alla nave, l'apertura dopo il tempo di volo
            if (notte && tempo_ >= n.prossimo_illuminante) {
                auto pz = std::find_if(n.pezzi.begin(), n.pezzi.end(), [&](const auto& p) {
                    return p.dati->calibro_mm >= CALIBRO_ILLUMINANTE_MM && !p.dati->raffica() && tempo_ >= p.prossimo && tempo_ >= p.pausa_fino;
                });
                if (pz != n.pezzi.end()) {
                    n.prossimo_illuminante = tempo_ + ILLUMINANTE_OGNI_S;
                    EventoArtiglieria e;
                    e.pos = {n.punto.x + S.caso.uniforme(-ILLUMINANTE_SPARSO_M, ILLUMINANTE_SPARSO_M), n.punto.y + S.caso.uniforme(-ILLUMINANTE_SPARSO_M, ILLUMINANTE_SPARSO_M)};
                    e.calibro_mm = pz->dati->calibro_mm;
                    e.carica_kg = 0.0;
                    e.tiratore = id;
                    e.bersaglio = bt->id;
                    EventoArtiglieria partenza = e;
                    partenza.tempo_s = tempo_;
                    partenza.tipo = TipoEventoArtiglieria::partenza;
                    partenza.pos = u.pos;
                    S.eventi_in_volo.push_back(partenza);
                    e.tempo_s = tempo_ + pz->dati->tempo_volo_s(distanza(u.pos, e.pos));
                    e.tipo = TipoEventoArtiglieria::illuminante;
                    e.quota_m = ILLUMINANTE_QUOTA_M;
                    e.durata_s = ILLUMINANTE_OGNI_S;
                    S.eventi_in_volo.push_back(e);
                    S.ultimo_illuminante = e.tempo_s;
                    S.punto_illuminante = e.pos;
                    pz->prossimo = tempo_ + pz->dati->intervallo_s();   // il colpo e' del pezzo: la sua cadenza
                }
            }
            const bool avvertimento = (bt->categoria == "nave" ? sn_navi : sn_sub) == Situazione::crisi;
            if (avvertimento) {   // tre colpi, poi una pausa
                if (n.avvertimenti >= AVVERTIMENTI_PER_SALVA) {
                    if (tempo_ - n.avvertimento_da < PAUSA_AVVERTIMENTI_S) continue;
                    n.avvertimenti = 0;
                }
                if (n.avvertimenti == 0) n.avvertimento_da = tempo_;
            }
            const double d = distanza(u.pos, n.punto);
            // fuoco amico: una nave amica della nave vicino alla linea di tiro e non molto oltre il bersaglio
            const double rlv = rilevamento(u.pos, n.punto);
            bool amico = false;
            for (const auto& [fid, f] : unita_) {
                if (fid == id || fid == bt->id || f.giocatore || f.distrutta || f.persa || (f.flag & 0x20) || (f.flag & 3) == 3 || ostili(f, u)) continue;
                if (f.categoria != "nave" && !(f.categoria == "sottomarino" && f.quota <= 1.0)) continue;
                if (distanza(u.pos, f.pos) > d + FUOCO_AMICO_OLTRE_M) continue;
                if (std::fabs(normalizza_180(rilevamento(u.pos, f.pos) - rlv)) < FUOCO_AMICO_GRADI) { amico = true; break; }
            }
            if (amico) continue;
            CorrezioniTiro corr;
            corr.stato_mare = stato_mare();
            corr.notte = notte;
            corr.illuminante = notte && tempo_ >= S.ultimo_illuminante && tempo_ - S.ultimo_illuminante < ILLUMINANTE_OGNI_S &&
                               distanza(S.punto_illuminante, n.punto) < 2000.0;
            const auto em = emcon_.find(id);
            corr.radar_tiro = em == emcon_.end() || em->second.stato() != StatoEmcon::silenzio;
            corr.velocita_nave_kn = u.velocita;
            corr.in_accostata = std::fabs(rateo_virata_vero(u)) > 0.5;
            corr.fattore_difficolta = principiante && bt->giocatore && vera.in_superficie ? 0.5 : 1.0;
            for (auto& p : n.pezzi) {
                if (tempo_ < p.prossimo || tempo_ < p.pausa_fino) continue;
                // il deposito e' finito (ricerca 36); contro un sottomarino ne resta un quarto per difendersi (STIMA, principale, 04/11/2026)
                const int deposito = bt->categoria == "sottomarino" ? p.dati->deposito * 3 / 4 : p.dati->deposito;
                if (p.dati->deposito > 0 && p.consumati >= deposito) continue;
                // contro le teste e gli alberi la portata del periscopio (0: mai, come il 16"; il principiante non ci spara)
                const double portata = vera.in_superficie ? p.dati->portata_superficie_m : principiante && bt->giocatore ? 0.0 : p.dati->portata_periscopio_m;
                if (portata <= 0.0 || d > portata || d < p.dati->portata_minima_m) continue;
                corr.primo_colpo = p.sparati < COLPI_AGGIUSTAMENTO;   // il tiro d'aggiustamento
                for (int k = 0; k < std::max(1, p.dati->canne); ++k) {
                    auto colpi = spara(*p.dati, u.pos, n.punto, vel, vera, tempo_, corr, S.caso, avvertimento);
                    // quelli sparati, non quelli a segno: spara() rende solo i proietti che toccano, e un rimbalzo e' un proietto solo
                    p.consumati += p.dati->raffica() ? std::max(1, p.dati->raffica_colpi) : 1;
                    const int sparo = ++S.spari;
                    for (std::size_t j = 0; j < colpi.size(); ++j) {
                        colpi[j].tiratore = id;
                        colpi[j].bersaglio = bt->id;
                        colpi[j].sparo = sparo;
                        for (auto e : eventi_di(colpi[j], u.pos, j == 0)) {
                            e.tiratore = id;
                            S.eventi_in_volo.push_back(e);
                        }
                        S.in_volo.push_back(colpi[j]);
                    }
                }
                ++p.sparati;
                p.prossimo = tempo_ + p.dati->intervallo_s();
                if (--p.pronti <= 0) {   // il tamburo o il raffreddamento
                    p.pronti = p.dati->colpi_pronti;
                    p.pausa_fino = tempo_ + p.dati->ricarica_s;
                }
                if (avvertimento && ++n.avvertimenti >= AVVERTIMENTI_PER_SALVA) break;
            }
        }
    }
    // gli impatti e gli eventi arrivati: nelle liste pubbliche e, gli eventi, nella scena acustica (G5)
    const auto arrivati = [&](auto& in_volo, auto& lista, auto tempo) {
        std::stable_sort(in_volo.begin(), in_volo.end(), [&](const auto& a, const auto& b) { return tempo(a) < tempo(b); });
        std::size_t k = 0;
        for (; k < in_volo.size() && tempo(in_volo[k]) <= tempo_; ++k) lista.push_back(in_volo[k]);
        in_volo.erase(in_volo.begin(), in_volo.begin() + static_cast<std::ptrdiff_t>(k));
        return k;
    };
    const std::size_t caduti = arrivati(S.in_volo, impatti_artiglieria_, [](const ImpattoArtiglieria& i) { return i.tempo_impatto_s; });
    bool bombe = false;
    for (std::size_t k = impatti_artiglieria_.size() - caduti; k < impatti_artiglieria_.size(); ++k) {
        // l'esito si decide allo sparo: se intanto il bersaglio e' affondato, o un sottomarino e' sceso sotto, il colpo
        // in volo cade in acqua
        if (ImpattoArtiglieria& x = impatti_artiglieria_[k];
            x.esito == ImpattoArtiglieria::Esito::diretto || x.esito == ImpattoArtiglieria::Esito::schegge) {
            const auto b = unita_.find(x.bersaglio);
            const bool via = b == unita_.end() || b->second.distrutta || b->second.persa || b->second.affondamento > 0 ||
                             (b->second.categoria == "sottomarino" && sagoma_esposta(b->second).vuota());
            if (via) {
                x.esito = ImpattoArtiglieria::Esito::acqua;
                x.parte = ParteEsposta::nessuna;
                x.distanza_parte_m = -1.0;
            }
        }
        const ImpattoArtiglieria& i = impatti_artiglieria_[k];
        bombe |= g && i.bersaglio == g->id && i.pezzo == "bomba_avvertimento";
        // un sottomarino dell'IA sotto il tiro si immerge: alberi giu' e sotto la quota sicura (22 §9.8)
        if (i.avvertimento) continue;
        if (auto b = unita_.find(i.bersaglio); b != unita_.end() && !b->second.giocatore && b->second.categoria == "sottomarino" &&
                                               distanza(b->second.pos, i.punto) <= TIRO_VICINO_M) {
            UnitaClassica& v = b->second;
            v.flag54 &= ~6;
            if (v.quota_ordinata < QUOTA_SICURA_IA_M) v.quota_ordinata = std::min(QUOTA_SICURA_IA_M, v.quota_max);
            v.rateo_quota = std::max(v.rateo_quota, v.rateo_quota_modello);
        }
    }
    if (bombe) messaggio_di_bordo(g->id, MSG_BOMBE_AVVERTIMENTO);
    const std::size_t nuovi = arrivati(S.eventi_in_volo, eventi_artiglieria_, [](const EventoArtiglieria& e) { return e.tempo_s; });
    if (catalogo_)
        for (std::size_t k = eventi_artiglieria_.size() - nuovi; k < eventi_artiglieria_.size(); ++k) {
            const int base = TRANSITORI_DA + 10 * (S.prossimo_transitorio % TRANSITORI_QUANTI);
            const int seme = SEME_DA + 10 * S.prossimo_transitorio;
            S.prossimo_transitorio = (S.prossimo_transitorio + 1) % SEMI_QUANTI;
            for (auto& t : transitori_artiglieria(eventi_artiglieria_[k], base, seme_ ^ static_cast<std::uint64_t>(seme)))
                transitori_armi_.push_back(std::move(t));
        }
}

// La cassa zavorra squarciata dai colpi a prua o a poppa (il modello e' del kit: DanniSottomarini, la falla e la cassa
// che non tiene piu' l'aria; la principale, 31/10/2026). L'effetto in missione e' una scelta di comando, non un limite
// fisico: il sottomarino dell'IA in superficie, o che sta appena scendendo (sopra QUOTA_RINUNCIA_M), resta a galla,
// perche' senza quella riserva potrebbe non riemergere (STIMA), anche sotto il tiro (prima scendeva, 22 §9.8). Il
// giocatore sceglie da se': il rapporto del kit lo avvisa e la fisica fa il resto
void Missione::casse_squarciate_ia(const DanniSottomarini& sub) {
    constexpr double QUOTA_RINUNCIA_M = 10.0;
    for (auto& [id, u] : unita_) {
        if (u.giocatore || u.categoria != "sottomarino" || u.distrutta || u.persa) continue;
        const DanniArtiglieriaBattello* a = sub.artiglieria(id);
        if (!a || a->casse_squarciate <= 0 || u.quota > QUOTA_RINUNCIA_M) continue;
        u.quota_ordinata = 0.0;
        u.rateo_quota = std::max(u.rateo_quota, u.rateo_quota_modello);
    }
}

}  // namespace m688
