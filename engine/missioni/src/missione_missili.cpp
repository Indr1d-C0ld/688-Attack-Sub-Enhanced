// Missili dell'Enhanced nella missione (docs/21_stazione_missili.md §11.9; modulo puro nucleo/missili.hpp; nota
// condivisa missili-aggancio-runtime-proposta.md). Il collegamento:
// - la stazione: per tubo le impostazioni, la fonte e i dati del bersaglio che ha solo l'interfaccia (ordine
//   imposta_missile), la preparazione PWR/ALN (prepara_missile) e la slide valve per lato. Un missile caricato prima
//   della missione parte gia' preparato; dopo il primo lancio dal tubo, il prossimo va preparato (STIMA di gioco);
// - il piano sui dati del battello: la posizione stimata (navigazione incerta), il contatto designato come lo sa il
//   battello, i contatti noti (con la posizione, o con la soluzione della TMA dell'equipaggio); le fonti TMA (il MATE),
//   periscopio (lo stadimetro), ESM (solo il rilevamento) e radio (il quadro radio, geografico);
// - il lancio: un'unita' della missione (categoria arma, classe = il missile) mossa dal Missile, che parte dalla
//   posizione vera; il gestore di lancio della missione gira ancora, il suo ATTACK_RND no (Missione::attacco_da_missile);
// - a ogni passo le EcoNave, cioe' la verita' delle navi in superficie coi loro radar accesi, e gli eventi: il seeker
//   che emette (passo_elettronica), l'impatto con le difese della nave (nucleo/difese_navi) e il danno della testata.
// Il classico non lo usa: il catalogo si legge solo con i siluri dell'Enhanced.

#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <string>

#include "missioni/armi_ia_missione.hpp"
#include "missioni/missione.hpp"
#include "nucleo/elettronica.hpp"

extern "C" {
#include "lua.h"
}

namespace m688 {

namespace {

constexpr double SLIDE_STESSO_LATO_S = 20.0, SLIDE_ALTRO_LATO_S = 4.0;   // docs/21 §3.3: 15-25 s e 3-5 s (STIMA)
constexpr double SIGMA_MATE = 0.15;              // senza la sigma dal MATE: 15% della distanza (STIMA)
constexpr double SIGMA_STADIMETRO = 0.05;        // lo stadimetro: 5% della distanza (STIMA)
constexpr double SIGMA_POSIZIONE = 0.10;         // un'eco, una vista, un radar senza la sigma: 10% (STIMA)
constexpr double SIGMA_RILEVAMENTO_DEG = 1.0;    // attraverso il rilevamento
// il missile fermato dalla difesa di punto scoppia in aria prima della nave (STIMA: CIWS e SAM a corto raggio lo
// prendono negli ultimi 0,5-1,5 km), almeno qualche metro sul mare
constexpr double ABBATTUTO_PRIMA_M = 500.0, QUOTA_ABBATTUTO_MIN_M = 3.0;
constexpr double DANNO_HARPOON = 75.0, TESTATA_HARPOON_KG = 221.0, ESPLOSIVO_HARPOON_KG = 100.0;   // il danno dei gestori della libreria (ATTACK_RND 75)
constexpr int MSG_MISSILE_A_SEGNO = 50;
constexpr int MSG_ESERCITAZIONE_A_SEGNO = 456, MSG_ESERCITAZIONE_COLPITI = 457;   // come i siluri (missione.cpp)
constexpr int MSG_MISSILE_AGGANCIA = 479;   // «missile ha agganciato» (runtime, fascia 470-479)
int lato_di(const UnitaClassica& u) { return u.flag & 0x18; }   // 0 neutrali, 8 usa, 16 urss, 24 la parte del luogo (M7)
constexpr double KN = 1852.0 / 3600.0;
constexpr double BDA_TLAM_S = 1800.0;   // il rapporto sull'esito del TLAM arriva col broadcast, mezz'ora dopo (STIMA)
constexpr double VELOCITA_MAX_RAPPORTO_KN = 30.0;   // senza moto nel rapporto: il cerchio cresce come una nave a 30 kn
// il tiro antinave dell'IA senza script (passo_missili_ia): salve, intervalli e riserve (STIMA)
constexpr double MISSILI_IA_OGNI_S = 1.0;
constexpr double CONTATTO_RECENTE_MISSILE_S = 30.0;   // il contatto aggiornato da poco: la posizione per il piano
constexpr double QUOTA_IN_SUPERFICIE_M = 3.0;         // il sottomarino con lo scafo fuori (come la TMA dell'IA)
constexpr int SALVA_SU_NAVE = 2, SALVA_SU_SOTTOMARINO = 1;
constexpr double FRA_MISSILI_SALVA_S = 4.0;           // i lanci della salva a pochi secondi (STIMA: 2-5 s)
constexpr double DOPO_SALVA_S = 120.0;                // dalla fine dei missili, la valutazione del danno prima di un'altra salva
constexpr double REAZIONE_MISSILI_IA_S = 60.0;        // dal bersaglio possibile al primo lancio (STIMA: designazione e
                                                      // preparazione della nave in allarme)
constexpr double RIPROVA_MISSILI_IA_S = 20.0;         // senza un piano valido (fuori portata), si riprova
constexpr double DURATA_CLASSIFICATO_MISSILE = 1400.0;   // il contatto lungo al radar: la classe nota (come il giocatore, 1400)
// la traccia solida con la posizione tenuta da tanto vale come classificata: il centro di valutazione la classifica dal
// comportamento, dalle dimensioni e dal quadro della forza (STIMA, come il «noto dopo 1400» del classico)
constexpr double TRACCIA_CLASSIFICATA_S = 300.0;
constexpr double VELOCITA_MERCANTILE_KN = 18.0;      // oltre la crociera di un mercantile (STIMA)
constexpr double MANOVRA_ROTTA_GRADI = 5.0, MANOVRA_VELOCITA_KN = 1.0;   // un'accostata o un cambio di macchina in corso
constexpr double FORMAZIONE_M = 10000.0;             // entro questa distanza da un'unita' militare nota della stessa parte
constexpr double AMICI_SUL_RADAR_M = 40000.0;         // le navi amiche che una nave sa dove sono senza un contatto (STIMA: radar di ricerca e rete)
constexpr double MANOVRA_LANCIO_MAX_S = 900.0;        // il sottomarino che non arriva alla quota e alla velocita' di lancio rinuncia
constexpr double FRAZIONE_VELOCITA_LANCIO = 0.6;      // sotto la velocita' massima di lancio, con margine
constexpr double RICARICA_TUBO_S = 300.0;             // il tubo del missile si ricarica come per i siluri (missione_armi_ia)
int riserva_missili_nave(const std::string& classe) {   // i missili antinave a bordo (STIMA dalle classi di servizio)
    static const std::map<std::string, int> r = {{"slava", 16}, {"kirov", 20}, {"nanuchka", 6}, {"perry", 4},
                                                 {"spruance", 8}, {"kidd", 8}, {"iowa", 16}};
    const auto it = r.find(classe);
    return it != r.end() ? it->second : 4;
}

int lato_tubo(int tubo) { return (tubo - 1) % 2; }   // 688: 1 e 3 a dritta, 2 e 4 a sinistra (il 7 col primo lato)

Vec2 lungo(Vec2 da, double rilevamento, double d) {
    const double r = rilevamento * 3.14159265358979323846 / 180.0;
    return {da.x + d * std::sin(r), da.y + d * std::cos(r)};
}

// le classi credute da chi lancia. Il battello: l'ultima classificazione del registro (del giocatore o del sonarista) per
// contatto. L'IA: la sua identificazione visiva, se no il suo sonarista (come lato_creduto); il registro non e' suo
std::map<int, std::string> classi_credute(const Missione& m, const UnitaClassica& l) {
    std::map<int, std::string> c;
    if (l.giocatore) {
        for (const auto& e : m.registro().eventi())
            if (e.tipo == TipoEvento::classificazione && !e.classe_creduta.empty()) c[e.unita] = e.classe_creduta;
        return c;
    }
    const Sonarista* s = m.sonarista_ia(l.id);
    for (const auto& [id, k] : l.contatti) {
        if (const auto* v = m.identificazione_visiva(l.id, id); v && !v->classe.empty()) c[id] = v->classe;
        else if (const TracciaSonar* t = s ? s->traccia_di(id) : nullptr; t && t->stato == TracciaSonar::Stato::classificata && !t->classe.empty()) c[id] = t->classe;
    }
    return c;
}

// la scatola pulita, come la vede il giocatore sulla carta (docs/26): oltre al bersaglio, nell'area di ricerca del seeker
// nessun contatto creduto neutrale, amico o di parte ignota (il seeker prende il primo eco, di chiunque). La regola dell'IA:
// il giocatore ha gli stessi avvisi, e tira lo stesso se vuole. `parte` da' la parte creduta da chi lancia
template <class Parte>
bool scatola_pulita(const PianoTiro& p, int bersaglio, Parte&& parte) {
    const auto in = [](const std::vector<int>& v, int c) { return std::find(v.begin(), v.end(), c) != v.end(); };
    return std::none_of(p.in_area.begin(), p.in_area.end(), [&](int c) {
        return c != bersaglio && (in(p.neutrali_in_area, c) || in(p.amici_in_area, c) || parte(c) <= 0);
    });
}

}  // namespace

const DatiMissile* Missione::missile_del_tubo(int tubo) const {
    if (!catalogo_missili_ || tubo < 1 || tubo > static_cast<int>(tubi_.size())) return nullptr;
    const std::string& f = tubi_[static_cast<std::size_t>(tubo - 1)];
    // le famiglie del carico (carico.toml) o l'arma: harpoon -> 84A o 84C per l'anno, tomahawk -> il TASM, attacco_terra
    // -> il TLAM-C
    const char* arma = f == "harpoon" ? "harpoon" : f == "tomahawk" || f == "tasm" ? "tasm" : f == "attacco_terra" || f == "tlam_c" ? "tlam_c" : nullptr;
    return arma ? catalogo_missili_->per_arma(arma, anno_) : nullptr;
}

int Missione::dimensione_creduta(const std::string& classe) const {
    const auto it = dimensioni_classi_.find(classe);
    return it == dimensioni_classi_.end() || it->second.lunghezza_m <= 0.0 ? 1 : dimensione_da_lunghezza(it->second.lunghezza_m);
}

StatoLanciatore Missione::lanciatore_missile(const UnitaClassica& g) const {
    StatoLanciatore l;
    l.pos = posizione_stimata(g.id);
    if (const auto e = ellisse_navigazione(g.id)) l.navigazione = *e;
    l.quota_m = g.quota;
    l.velocita_kn = std::fabs(g.velocita);
    l.rotta = g.rotta;
    l.stato_mare = stato_mare();
    return l;
}

// Il contatto designato come lo sa il battello, per la fonte scelta. Le posizioni sono nel mondo del battello: la sua
// stima piu' il relativo (contro un bersaglio del proprio sonar l'errore di navigazione non conta)
std::optional<BersaglioStimato> Missione::bersaglio_missile(const UnitaClassica& g, const TiroTubo& t) const {
    if (g.bersaglio < 0) return std::nullopt;
    const auto credute = classi_credute(*this, g);
    // la radio: il dato oltre l'orizzonte, senza un contatto proprio (il quadro radio della forza)
    if (t.fonte == FonteDati::radio) {
        std::optional<BersaglioStimato> rr;
        std::string classe;
        if (g.giocatore && radio_) {   // il giocatore: i contatti d'intelligence della sala radio (rapporti della forza, OTH)
            for (const ContattoIntelligence& ci : radio_->intelligence(tempo_)) {
                if (ci.dati.falso || std::find(ci.dati.unita.begin(), ci.dati.unita.end(), g.bersaglio) == ci.dati.unita.end()) continue;
                if (rr && ci.dati.t_oss <= rr->t_pos) continue;   // il piu' fresco
                const bool moto = ci.dati.rotta_err_deg < 90.0;   // la forza non da' il moto (180 gradi d'errore)
                rr = da_rapporto(g.bersaglio, ci.dati.pos, ci.dati.t_oss, ci.dati.errore_m,
                                 moto ? std::optional<double>(ci.dati.rotta) : std::nullopt,
                                 moto ? std::optional<double>(ci.dati.velocita_kn * KN) : std::nullopt, VELOCITA_MAX_RAPPORTO_KN * KN);
                if (moto) {
                    rr->sigma_rotta_deg = ci.dati.rotta_err_deg;
                    rr->sigma_velocita_m_s = ci.dati.velocita_err_kn * KN;
                }
            }
        } else if (const auto cr = contatto_radio(g.id, g.bersaglio); cr && cr->posizione_valida && cr->r.pos) {   // l'IA: il quadro
            rr = da_rapporto(g.bersaglio, *cr->r.pos, tempo_, cr->raggio_m, std::nullopt, std::nullopt, VELOCITA_MAX_RAPPORTO_KN * KN);
            classe = cr->r.classe;
        }
        if (!rr) return std::nullopt;
        BersaglioStimato r = *rr;
        if (const auto cc = credute.find(g.bersaglio); cc != credute.end()) r.dimensione = dimensione_creduta(cc->second);
        else if (!classe.empty()) r.dimensione = dimensione_creduta(classe);   // la classe del rapporto
        if (t.rotta && t.velocita_kn) {   // il moto, se il giocatore l'ha messo sul pannello
            r.moto_noto = true;
            r.rotta = *t.rotta;
            r.velocita_m_s = *t.velocita_kn * KN;
        }
        return r;
    }
    const auto k = g.contatti.find(g.bersaglio);
    if (k == g.contatti.end()) return std::nullopt;
    const Contatto& c = k->second;
    const Vec2 err = errore_navigazione(g.id), io{g.pos.x + err.x, g.pos.y + err.y};
    BersaglioStimato b;
    b.fonte = t.fonte;
    b.contatto = g.bersaglio;
    b.t_pos = tempo_;
    b.relativo = true;
    // la classe creduta, non la vera: il pannello sa cio' che sa il battello
    if (const auto cc = credute.find(g.bersaglio); cc != credute.end()) b.dimensione = dimensione_creduta(cc->second);
    const auto metti = [&](Vec2 pos, double sigma_lungo, double sigma_attraverso) {
        b.pos = pos;
        b.distanza_nota = true;
        b.rilevamento = rilevamento(io, pos);
        b.ellisse.maggiore_m = std::max(sigma_lungo, sigma_attraverso);
        b.ellisse.minore_m = std::min(sigma_lungo, sigma_attraverso);
        const double asse = sigma_lungo >= sigma_attraverso ? b.rilevamento : b.rilevamento + 90.0;
        b.ellisse.orientamento_deg = std::fmod(std::fmod(asse, 180.0) + 180.0, 180.0);
    };
    const auto solo_rilevamento = [&](double ril) {
        b.distanza_nota = false;
        b.rilevamento = ril;
    };
    switch (t.fonte) {
        case FonteDati::tma:   // i quadranti del MATE, se l'interfaccia li ha mandati; se no la posizione del contatto
            if (t.distanza_m && *t.distanza_m > 0.0) {
                const double d = *t.distanza_m;
                metti(lungo(io, c.rilevamento, d), t.sigma_distanza_m.value_or(SIGMA_MATE * d),
                      t.sigma_trasversale_m.value_or(d * std::sin(SIGMA_RILEVAMENTO_DEG * 3.14159265358979323846 / 180.0)));
            } else if (c.posizione_nota) {
                const Vec2 p{c.pos_stimata.x + err.x, c.pos_stimata.y + err.y};
                const double d = distanza(io, p);
                metti(p, SIGMA_POSIZIONE * d, d * std::sin(SIGMA_RILEVAMENTO_DEG * 3.14159265358979323846 / 180.0));
            } else if (const StimaTma* st = [&]() -> const StimaTma* {   // senza il MATE: la TMA dell'equipaggio, o dell'IA
                           if (g.giocatore) {
                               const auto te = tma_equipaggio_.find(g.bersaglio);
                               return te != tma_equipaggio_.end() && te->second.stima && te->second.stima->osservabile ? &*te->second.stima : nullptr;
                           }
                           const auto ti = tma_ia_.find({g.id, g.bersaglio});
                           return ti != tma_ia_.end() && ti->second.stima && ti->second.stima->osservabile ? &*ti->second.stima : nullptr;
                       }()) {
                metti({st->posizione.x + err.x, st->posizione.y + err.y}, std::max(st->sigma_distanza_m, 100.0), std::max(st->sigma_trasversale_m, 50.0));
            } else {
                solo_rilevamento(c.rilevamento);
            }
            break;
        case FonteDati::periscopio:   // lo stadimetro: la distanza dall'interfaccia, il rilevamento della vista
            if (t.distanza_m && *t.distanza_m > 0.0) {
                const double d = *t.distanza_m;
                metti(lungo(io, c.rilevamento, d), t.sigma_distanza_m.value_or(SIGMA_STADIMETRO * d),
                      d * std::sin(0.5 * 3.14159265358979323846 / 180.0));
            } else {
                return std::nullopt;
            }
            break;
        case FonteDati::esm:   // solo il rilevamento dell'intercettazione: solo BOL
            if (c.sensore != "esm" && !c.posizione_nota && c.qualita <= 0.0) return std::nullopt;
            solo_rilevamento(c.rilevamento);
            break;
        case FonteDati::radio:   // sopra: senza il contatto proprio
            return std::nullopt;
    }
    if (t.rotta && t.velocita_kn) {
        b.moto_noto = true;
        b.rotta = *t.rotta;
        b.velocita_m_s = *t.velocita_kn * KN;
    }
    return b;
}

std::vector<ContattoNoto> Missione::contatti_noti_missile(const UnitaClassica& g) const {
    std::vector<ContattoNoto> v;
    const Vec2 err = errore_navigazione(g.id);
    const auto credute = classi_credute(*this, g);
    const int mia = lato_di(g);
    for (const auto& [id, c] : g.contatti) {
        if (!c.visibile()) continue;
        // la posizione: la vista, il radar, l'eco; o la soluzione della TMA dell'equipaggio (del giocatore) o della sua
        // (l'IA). Senza, non si sa se cade nell'area
        std::optional<Vec2> pos;
        if (c.posizione_nota) pos = c.pos_stimata;
        else if (g.giocatore) {
            if (const auto t = tma_equipaggio_.find(id); t != tma_equipaggio_.end() && t->second.stima && t->second.stima->osservabile)
                pos = t->second.stima->posizione;
        } else if (const auto t = tma_ia_.find({g.id, id}); t != tma_ia_.end() && t->second.stima && t->second.stima->osservabile) {
            pos = t->second.stima->posizione;
        }
        if (!pos) continue;
        const auto ui = unita_.find(id);
        const UnitaClassica* u = ui == unita_.end() ? nullptr : &ui->second;
        if (!u || u->distrutta || u->persa || (u->flag & 0x20) || (u->flag & 3) == 3) continue;
        ContattoNoto n;
        n.id = id;
        n.pos = {pos->x + err.x, pos->y + err.y};
        // la classe e la parte credute: i neutrali dichiarati dalla missione si sanno dal briefing, i civili solo
        // classificati come tali
        const auto cc = credute.find(id);
        n.dimensione = cc == credute.end() ? 1 : dimensione_creduta(cc->second);
        bool civile = false;
        if (cc != credute.end()) {
            UnitaClassica creduta = *u;
            creduta.id = -1;
            creduta.classe = cc->second;
            civile = categoria_roe(creduta) == CategoriaRoe::mercantili;
        }
        n.neutrale_creduto = neutrali_.count(id) || civile;
        // l'amico: per l'IA la parte che crede; il giocatore lo sa solo dalla classe che ha dato al contatto (l'avviso non
        // deve dirgli di chi e' un contatto che non ha classificato)
        if (g.giocatore)
            n.amico_creduto = mia != 0 && cc != credute.end() &&
                              std::any_of(unita_.begin(), unita_.end(), [&](const auto& k) { return k.second.classe == cc->second && lato_di(k.second) == mia; });
        else
            n.amico_creduto = mia != 0 && lato_creduto(g, *u) == mia;
        v.push_back(n);
    }
    // le navi amiche non entrano nei contatti radar dell'IA (radar_contro_nave), ma la nave le ha sul radar e sulla rete
    if (!g.giocatore && g.categoria == "nave" && mia != 0)
        for (const auto& [id, u] : unita_) {
            const bool superficie = u.categoria == "nave" || (u.categoria == "sottomarino" && u.quota <= 2.0);
            if (id == g.id || !superficie || u.distrutta || u.persa || (u.flag & 0x20) || (u.flag & 3) == 3 || (u.flag & 0x80)) continue;
            if (parte_nota(g, u) != mia || distanza(g.pos, u.pos) > AMICI_SUL_RADAR_M) continue;
            if (std::any_of(v.begin(), v.end(), [&, i = id](const ContattoNoto& k) { return k.id == i; })) continue;   // gia' fra i contatti con la posizione
            ContattoNoto n;
            n.id = id;
            n.pos = {u.pos.x + err.x, u.pos.y + err.y};
            n.dimensione = dimensione_creduta(u.classe);
            n.amico_creduto = true;
            v.push_back(n);
        }
    return v;
}

bool Missione::slide_libera(int tubo) const {
    return tempo_ >= slide_dal_[static_cast<std::size_t>(lato_tubo(tubo))];
}

std::optional<Missione::TiroMissile> Missione::tiro_missile(int tubo) const {
    const DatiMissile* d = missile_del_tubo(tubo);
    if (!d) return std::nullopt;
    TiroMissile r;
    r.tubo = tubo;
    r.dati = d;
    static const TiroTubo vuoto;
    const auto it = tiri_.find(tubo);
    const TiroTubo& t = it == tiri_.end() ? vuoto : it->second;
    r.fonte = t.fonte;
    r.imp = t.imp;
    if (t.prep && t.missile == d->id) {
        r.fase = t.prep->fase();
        r.residuo_s = t.prep->residuo_s();
        r.pwr = t.prep->pwr();
        r.aln = t.prep->aln();
    } else if (!tubi_usati_.count(tubo) && d->ruolo != RuoloMissile::attacco_terra) {
        // Harpoon e TASM caricati prima della missione: gia' preparati (pre-caricati e pre-allineati, dossier 05). Il
        // TLAM no: la sua ora di preparazione resta vera sempre (decisione 3 di docs/21 §10)
        r.fase = FasePreparazione::pronto;
        r.pwr = r.aln = true;
    }
    r.slide_libera = slide_libera(tubo);
    const auto* g = giocatore();
    if (d->ruolo == RuoloMissile::attacco_terra) {   // TLAM: la missione della libreria sul pannello, sulla stima
        r.missione_tlam = t.missione_tlam;
        if (t.missione_tlam >= 0 && t.missione_tlam < static_cast<int>(libreria_tlam_.size())) {
            r.tlam = &libreria_tlam_[static_cast<std::size_t>(t.missione_tlam)];
            r.ordine_esecuzione = tlam_eseguibili_.count(r.tlam->id) != 0;
            if (radio_ && !r.ordine_esecuzione)   // I12: e' arrivato solo un ordine col gruppo che non torna
                for (const auto& rv : radio_->ricevuti())
                    if (const auto e = esecuzioni_tlam_.find(rv.messaggio.id); !rv.messaggio.autentico && e != esecuzioni_tlam_.end() && e->second == r.tlam->id)
                        r.ordine_non_autentico = true;
            if (g) r.stato_tlam = valuta_tlam(*d, *r.tlam, posizione_stimata(g->id), sigma_navigazione_m(g->id).value_or(0.0), tempo_);
        }
        const bool valida = r.stato_tlam && (r.stato_tlam->validita == ValiditaTlam::valida || r.stato_tlam->validita == ValiditaTlam::ripianifica);
        r.rdy = valida && r.aln && r.ordine_esecuzione;
        return r;
    }
    if (g) {
        r.bersaglio = bersaglio_missile(*g, t);
        if (r.bersaglio) r.piano = pianifica(*d, t.imp, *r.bersaglio, lanciatore_missile(*g), contatti_noti_missile(*g), tempo_);
    }
    r.rdy = r.piano.valido && r.aln;
    return r;
}

std::vector<Missione::MissileInVolo> Missione::missili_in_volo() const {
    std::vector<MissileInVolo> v;
    for (const auto& [id, x] : missili_) {
        if (!x.giocatore || !x.missile.attivo()) continue;
        MissileInVolo m;
        m.unita = id;
        m.tubo = x.tubo;
        m.missile = x.missile.dati().id;
        m.t_lancio = x.t_lancio;
        m.piano = x.piano;
        m.imp = x.imp;
        m.stima = stima_volo(x.piano, x.imp, x.missile.dati(), tempo_ - x.t_lancio);
        v.push_back(std::move(m));
    }
    return v;
}

bool Missione::imposta_missile(const Ordine& o) {
    if (!catalogo_missili_ || o.n < 1 || o.n > 7) return false;
    TiroTubo& t = tiri_[o.n];
    if (o.tipo == TipoOrdine::prepara_missile) {
        const DatiMissile* d = missile_del_tubo(o.n);
        if (!d) return false;
        if (o.a != 0.0) {
            if (!t.prep || t.missile != d->id) {
                // il TLAM: il primo di bordo 15 minuti, gli altri in parallelo coi minuti di preparazione_altri_s
                bool primo = true;
                if (d->ruolo == RuoloMissile::attacco_terra)
                    for (const auto& [k, x] : tiri_)
                        if (k != o.n && x.prep && x.prep->fase() != FasePreparazione::spento && x.missile == d->id) primo = false;
                t.prep.emplace(*d, primo);
                t.missile = d->id;
                t.prep->avvia();
                if (!tubi_usati_.count(o.n) && d->ruolo != RuoloMissile::attacco_terra) t.prep->avanza(1e9);   // gia' pronto
            }
            t.prep->avvia();
        } else if (t.prep) {
            t.prep->spegni();
        }
        return true;
    }
    const auto valore = [&]() -> std::optional<double> { return o.a < 0.0 ? std::nullopt : std::optional<double>(o.a); };
    switch (static_cast<CampoMissile>(static_cast<int>(o.b))) {
        case CampoMissile::fonte: t.fonte = static_cast<FonteDati>(std::clamp(static_cast<int>(o.a), 0, 3)); break;
        case CampoMissile::modo: t.imp.modo = o.a != 0.0 ? ModoLancio::bol : ModoLancio::rbl; break;
        case CampoMissile::taglia: t.imp.taglia = static_cast<Taglia>(std::clamp(static_cast<int>(o.a), 0, 2)); break;
        case CampoMissile::srch_m: t.imp.srch_m = std::max(0.0, o.a); break;
        case CampoMissile::dstr_m: t.imp.dstr_m = std::max(0.0, o.a); break;
        case CampoMissile::missione_tlam:   // < 0: nessuna; oltre la libreria: rifiutato
            if (o.a >= static_cast<double>(libreria_tlam_.size())) return false;
            t.missione_tlam = o.a < 0.0 ? -1 : static_cast<int>(o.a);
            break;
        case CampoMissile::distanza_m: t.distanza_m = valore(); break;   // < 0: dimentica
        case CampoMissile::rotta: t.rotta = valore(); break;
        case CampoMissile::velocita_kn: t.velocita_kn = valore(); break;
        case CampoMissile::sigma_distanza_m: t.sigma_distanza_m = valore(); break;
        case CampoMissile::sigma_trasversale_m: t.sigma_trasversale_m = valore(); break;
        default: return false;
    }
    return true;
}

// Il lancio dal tubo: il piano del momento sui dati del battello, poi il missile dalla posizione vera. Falso senza un
// piano valido (la stazione lo sa prima: TiroMissile::rdy)
bool Missione::lancia_missile_modello(UnitaClassica& g, int tubo) {
    const DatiMissile* d = missile_del_tubo(tubo);
    if (!d) return false;
    TiroTubo& t = tiri_[tubo];
    const auto b = bersaglio_missile(g, t);
    if (!b) return false;
    const PianoTiro p = pianifica(*d, t.imp, *b, lanciatore_missile(g), contatti_noti_missile(g), tempo_);
    if (!p.valido) return false;
    crea_missile(g, *d, t.imp, p, b->contatto, tubo);
    const auto lato = static_cast<std::size_t>(lato_tubo(tubo));
    slide_dal_[lato] = tempo_ + SLIDE_STESSO_LATO_S;
    slide_dal_[1 - lato] = std::max(slide_dal_[1 - lato], tempo_ + SLIDE_ALTRO_LATO_S);
    t.prep.reset();   // il tubo e' vuoto: il prossimo missile si prepara da capo
    t.missile.clear();
    tubi_usati_.insert(tubo);
    return true;
}

// L'unita' del missile e il Missile: dalla posizione vera del lanciatore, col piano del suo mondo
void Missione::crea_missile(UnitaClassica& l, const DatiMissile& d, const ImpostazioniMissile& imp, const PianoTiro& p, int bersaglio,
                            int tubo) {
    UnitaClassica e;
    e.id = prossimo_id_++;
    e.classe = d.id;
    e.categoria = "arma";
    e.flag = 0x20 | 3 | (l.flag & 0x18);   // un'arma in aria, della parte del lanciatore
    e.pos = l.pos;
    e.quota = e.quota_ordinata = l.quota;
    e.rotta = e.rotta_ordinata = p.rotta;
    e.integrita = e.integrita_max = 2;
    e.collegata = e.lanciatore = l.id;
    e.bersaglio = bersaglio;
    ++create_;
    unita_[e.id] = e;
    // schede I03, S01 e S13: in esercitazione il missile e' inerte come i siluri (niente testata, il colpo si registra)
    if (esercitazione_tutte_ || lanciatori_esercitazione_.count(l.id)) armi_esercitazione_.insert(e.id);
    // il lancio per gli effetti (la fiammata al periscopio, il profilo dell'arma; partita.cpp): lo script MISSILE l'ha gia'
    // messo in lanci_missili_ nello stesso istante, e allora la voce prende l'arma e la rotta del modello; se no (la
    // stazione di tiro, l'IA senza script) una voce nuova
    auto voce = std::find_if(lanci_missili_.rbegin(), lanci_missili_.rend(),
                             [&](const LancioMissile& x) { return x.unita == l.id && std::fabs(x.tempo - tempo_) < 1e-6; });
    if (voce == lanci_missili_.rend()) {
        LancioMissile x;
        x.tempo = tempo_;
        x.unita = l.id;
        x.pos = l.pos;
        x.quota = l.quota;
        x.classe_lanciatore = l.classe;
        x.categoria_lanciatore = l.categoria;
        lanci_missili_.push_back(x);
        voce = lanci_missili_.rbegin();
    }
    voce->bersaglio = bersaglio;
    voce->rotta = normalizza_360(p.rotta);
    voce->arma = d.id;
    voce->missile = e.id;
    if (const auto* b = bersaglio >= 0 ? trova(bersaglio) : nullptr) voce->distanza_bersaglio_m = distanza(l.pos, b->pos);
    if (l.giocatore)   // i neutrali nella scatola di ricerca del piano (docs/26: «scatola pulita»)
        for (const int c : p.in_area)
            if (const auto* n = trova(c); n && c != bersaglio && ((n->flag & 0x18) == 0 || del_traffico(c))) ++voce->neutrali_scatola;
    const std::uint64_t seme = seme_ ^ (0x4D155111ull + static_cast<std::uint64_t>(e.id) * 0x9E37ull);
    const auto nuovo = missili_.emplace(e.id, MissileMissione{Missile(d, imp, p, l.pos, l.quota, std::fabs(l.velocita), l.id, seme), tubo, tempo_,
                                                              l.giocatore, p, imp});
    nuovo.first->second.forze_al_lancio = livelli_forze();
}

// Il MISSILE di uno script dell'IA (docs/21 §7, simmetria): il missile antinave della classe (per_classe), il bersaglio
// dello script come lo sa l'unita' (TMA, posizione, o il solo rilevamento: BOL), lo stesso pianifica del giocatore. Falso
// se la classe non ha missili antinave nel catalogo o il piano non e' valido: allora resta l'ATTACK_RND dello script
bool Missione::lancia_missile_ia(UnitaClassica& l) {
    const DatiMissile* d = catalogo_missili_->per_classe(l.classe, anno_);
    if (!d || d->ruolo != RuoloMissile::antinave) return false;
    // un antinave non parte su un sottomarino immerso (la principale, 31/10/2026: nella SCENE8C la Perry e la Kidd tiravano
    // l'Harpoon sull'Alfa in immersione)
    if (const UnitaClassica* x = trova(l.bersaglio); x && x->categoria == "sottomarino" && x->quota > 1.0) return false;
    TiroTubo t;   // l'IA: la posizione del contatto o la sua TMA (la fonte TMA senza quadranti), RBL; senza distanza BOL
    const auto b = bersaglio_missile(l, t);
    if (!b) return false;
    if (!b->distanza_nota) t.imp.modo = ModoLancio::bol;
    const PianoTiro p = pianifica(*d, t.imp, *b, lanciatore_missile(l), contatti_noti_missile(l), tempo_);
    if (!p.valido) return false;
    // anche lo script: dentro il convoglio le Perry non tirano (la principale, 04/11/2026, dalla S5: l'Harpoon di una Perry
    // sulla Perry vicina)
    if (!scatola_pulita(p, b->contatto, [&](int c) { const auto* x = trova(c); return x ? lato_creduto(l, *x) : 1; })) return false;
    crea_missile(l, *d, t.imp, p, b->contatto, 0);
    return true;
}

Missione::EsitoMissileScript Missione::missile_dello_script(int unita) {
    if (!catalogo_missili_) return EsitoMissileScript::senza_modello;
    UnitaClassica* l = trova(unita);
    if (!l || l->giocatore || l->distrutta || l->persa || (l->flag & 0x20)) return EsitoMissileScript::senza_modello;
    const DatiMissile* d = catalogo_missili_->per_classe(l->classe, anno_);
    if (!d || d->ruolo != RuoloMissile::antinave) return EsitoMissileScript::senza_modello;   // resta l'ATTACK_RND
    // l'esito e' del volo, o non c'e' (il piano non valido: il missile non parte): niente ATTACK_RND dopo
    lancio_modellato_ = true;
    return lancia_missile_ia(*l) ? EsitoMissileScript::lanciato : EsitoMissileScript::negato;
}

// Il tiro antinave dell'IA senza script (scheda I11; simmetria, docs/18): un'unita' dell'IA con il missile antinave
// della classe (per_classe) lo lancia da se', con lo stesso piano del giocatore.
// - Chi: le navi; i sottomarini lanciamissili (Oscar, Charlie II) e gli SSN occidentali con l'Harpoon nel carico dei
//   tubi (armi_ia_), dopo una manovra alla quota e alla velocita' di lancio del missile, che cede allo script e alla
//   difesa di quell'istante; finita la salva, la quota e la velocita' di prima.
// - Su chi: un bersaglio di superficie classificato (la classe del sonarista, l'identificazione visiva, il suo radar
//   identificato all'ESM, un contatto lungo; o una traccia tenuta 5 minuti di un contatto che si comporta da militare,
//   o qualunque traccia tenuta se la missione dichiara `zona_di_guerra = true`), una nave o un sottomarino in superficie visto da radar o vedetta, con le armi libere (armi_libere_ia: la
//   situazione della coppia, le ROE, la parte creduta); senza un contatto proprio, la nave del quadro radio con la
//   posizione e la classe (il dato oltre l'orizzonte). Mai su chi crede neutrale ne' sui non combattenti.
// - Come: un minuto di reazione; la salva e la riserva della classe (missili.toml, [[lanciatore]]; su un sottomarino
//   emerso sempre 1); poi l'attesa della fine dei missili e della valutazione.
// Solo nell'Enhanced con le forze e le armi dell'IA, mai nelle missioni originali (tengono i loro script).
void Missione::passo_missili_ia() {
    if (!armi_ia_ || armi_ia_->originale || !forze_ || tempo_ < prossimi_missili_ia_) return;
    prossimi_missili_ia_ = tempo_ + MISSILI_IA_OGNI_S;
    auto& A = *armi_ia_;
    // la chiave di missione `zona_di_guerra = true`: ogni traccia tenuta vale come classificata (niente neutrali in mare)
    lua_rawgeti(L_, LUA_REGISTRYINDEX, rif_missione_);
    lua_getfield(L_, -1, "zona_di_guerra");
    const bool zona_guerra = lua_toboolean(L_, -1);
    lua_pop(L_, 2);
    // i lanciatori con missili in volo (l'arma in aria della loro parte)
    std::set<int> in_volo;
    for (const auto& [mid, m] : missili_)
        if (const auto* w = trova(mid); w && !w->distrutta && !w->persa) in_volo.insert(w->lanciatore);
    // gli SSN occidentali: il tubo del carico con un missile, -1 se nessuno
    const auto tubo_missile = [&](int id) {
        const auto b = A.battelli.find(id);
        if (b == A.battelli.end()) return -1;
        for (std::size_t i = 0; i < b->second.carico.tubi.size(); ++i)
            if (const auto a = A.cat.armi.find(b->second.carico.tubi[i]); a != A.cat.armi.end() && a->second.tipo == TipoArmaCarico::missile)
                return static_cast<int>(i);
        return -1;
    };
    const auto fine_manovra = [](UnitaClassica& u, NaveMissiliIa& n) {
        if (!n.in_manovra) return;
        n.in_manovra = false;
        if (n.quota_prima >= 0.0) u.quota_ordinata = n.quota_prima;
        if (n.velocita_prima >= 0.0) u.velocita_ordinata = n.velocita_prima;
    };
    for (auto& [id, u] : unita_) {
        const bool sub = u.categoria == "sottomarino";
        if (u.giocatore || (u.categoria != "nave" && !sub) || u.distrutta || u.persa || u.affondamento > 0 || (u.flag & 0x20)) continue;
        const DatiMissile* d = catalogo_missili_->per_classe(u.classe, anno_);
        if (!d || d->ruolo != RuoloMissile::antinave) continue;
        const CatalogoMissili::Lanciatore* lc = catalogo_missili_->lanciatore(u.classe, anno_);
        auto& n = missili_ia_[id];
        // la riserva: quella della classe; gli occidentali senza riserva tirano dai tubi del loro carico
        const bool dal_carico = sub && (!lc || lc->riserva <= 0);
        if (n.rimasti < 0) n.rimasti = lc && lc->riserva > 0 ? lc->riserva : sub ? 0 : riserva_missili_nave(u.classe);
        if (dal_carico ? tubo_missile(id) < 0 : n.rimasti <= 0) { fine_manovra(u, n); continue; }
        if (n.salva == 0 && in_volo.count(id)) {   // l'esito della salva di prima, poi la valutazione, prima di un'altra
            fine_manovra(u, n);
            n.prossimo = std::max(n.prossimo, tempo_ + DOPO_SALVA_S);
            continue;
        }
        if (tempo_ < n.prossimo) continue;
        // il bersaglio: quello della salva in corso, se no il piu' vicino fra quelli possibili
        const auto lecito = [&](const UnitaClassica& t) {   // mai i non combattenti ne' chi crede neutrale
            return t.id != id && !t.distrutta && !t.persa && t.affondamento == 0 && !(t.flag & 0x20) && !(t.flag & 0x40) &&
                   lato_creduto(u, t) > 0;
        };
        // la classe nota: la vista, il radar del bersaglio identificato all'ESM, il sonarista (la sua classificazione,
        // giusta o creduta); il contatto lungo solo al radar. Il contatto lungo del solo sonar passivo non basta (la
        // principale, 30/10/2026: la Perry con la SQR-19 teneva per ore il cargo sovietico a 30 km e gli tirava 4 Harpoon)
        const auto classe_nota = [&](int cid, const Contatto& c) {
            if ((c.durata >= DURATA_CLASSIFICATO_MISSILE && c.sensore == "radar") || identificazione_visiva(id, cid)) return true;
            if (const auto e = esm_ia_.find(id); e != esm_ia_.end() && e->second)
                for (const Intercettazione& i : e->second->intercettazioni())
                    if (i.unita == cid && i.identificato) return true;
            if (const auto si = sonaristi_ia_.find(id); si != sonaristi_ia_.end())
                if (const TracciaSonar* tr = si->second.s->traccia_di(cid); tr && tr->stato == TracciaSonar::Stato::classificata) return true;
            return false;
        };
        // si comporta da unita' militare (decisione dell'utente, 29/10/2026: niente colpi sui mercantili non identificati):
        // oltre la crociera mercantile, in manovra adesso, o in formazione con un'unita' militare della stessa parte di
        // classe nota. Una volta giudicato, il candidato lo resta
        const auto militare = [&](int cid, const UnitaClassica& t) {
            if (zona_guerra || cid == n.candidato) return true;
            if (std::fabs(t.velocita) > VELOCITA_MERCANTILE_KN) return true;
            if (t.rateo_virata != 0.0 || std::fabs(normalizza_180(t.rotta - t.rotta_ordinata)) > MANOVRA_ROTTA_GRADI ||
                std::fabs(t.velocita - t.velocita_ordinata) > MANOVRA_VELOCITA_KN)
                return true;
            const int lt = lato_creduto(u, t);
            for (const auto& [kid, k] : u.contatti) {
                const auto* x = kid != cid ? trova(kid) : nullptr;
                if (!x || x->distrutta || x->persa || (x->flag & 0x20) || del_traffico(kid) || classe_civile(x->classe)) continue;
                if (distanza(x->pos, t.pos) <= FORMAZIONE_M && lato_creduto(u, *x) == lt && classe_nota(kid, k)) return true;
            }
            return false;
        };
        // classificato: la classe nota, o la traccia tenuta 5 minuti di un contatto che si comporta da militare (il centro
        // di valutazione), o qualunque traccia tenuta nella zona di guerra dichiarata dalla missione
        const auto classificato = [&](int cid, const Contatto& c) {
            if (classe_nota(cid, c)) return true;
            const auto* t = trova(cid);
            return t && tempo_ - c.prima_scoperta >= TRACCIA_CLASSIFICATA_S && militare(cid, *t);
        };
        // la classe creduta (la vista, il sonarista) di un mercantile: niente missili (decisione dell'utente, 29/10/2026, e
        // della principale, 30/10/2026: la Perry riconosceva il cargo come portarinfuse e gli tirava gli Harpoon)
        const auto creduto_civile = [&](int cid) {
            if (const auto* v = identificazione_visiva(id, cid); v && classe_civile(v->classe)) return true;
            if (const auto si = sonaristi_ia_.find(id); si != sonaristi_ia_.end())
                if (const TracciaSonar* tr = si->second.s->traccia_di(cid); tr && !tr->classe.empty() && classe_civile(tr->classe)) return true;
            return false;
        };
        const auto possibile = [&](int cid, const Contatto& c) {
            const auto* t = trova(cid);
            if (!t || !lecito(*t) || creduto_civile(cid)) return false;
            if (c.qualita < 800.0 || !c.posizione_nota || tempo_ - c.t_posizione > CONTATTO_RECENTE_MISSILE_S || !classificato(cid, c)) return false;
            if (t->categoria == "sottomarino") {
                if (t->quota > QUOTA_IN_SUPERFICIE_M || (c.sensore != "radar" && c.sensore != "vista")) return false;
            } else if (t->categoria != "nave") return false;
            return armi_libere_ia(id, cid).value_or(false);
        };
        int bid = -1;
        bool radio = false;
        if (n.salva > 0) {   // la salva continua finche' il bersaglio ha la posizione (la qualita' oscilla sulla soglia)
            const auto* t = trova(n.bersaglio);
            if (t && !t->distrutta && !t->persa && t->affondamento == 0) {
                if (const auto k = u.contatti.find(n.bersaglio); k != u.contatti.end() && k->second.posizione_nota) bid = n.bersaglio;
                else if (const auto cr = contatto_radio(id, n.bersaglio); cr && cr->posizione_valida) { bid = n.bersaglio; radio = true; }
            }
        }
        if (bid < 0) {
            n.salva = 0;
            double vicina = 1e18;
            for (const auto& [cid, c] : u.contatti)
                if (const double dd = distanza(u.pos, c.pos_stimata); dd < vicina && possibile(cid, c)) {
                    vicina = dd;
                    bid = cid;
                }
            if (bid < 0)   // oltre l'orizzonte: le navi classificate del quadro radio con la posizione
                for (const auto& [rb, cr] : quadro_radio(id)) {
                    const auto* t = trova(rb);
                    if (!t || t->categoria != "nave" || !lecito(*t) || !cr.posizione_valida || !cr.r.pos || cr.r.classe.empty() || u.contatti.count(rb)) continue;
                    if (const double dd = distanza(u.pos, *cr.r.pos); dd < vicina && armi_libere_ia(id, rb).value_or(false)) {
                        vicina = dd;
                        bid = rb;
                        radio = true;
                    }
                }
            if (bid < 0) { fine_manovra(u, n); continue; }
            if (bid != n.candidato) {   // un bersaglio nuovo: la designazione e la preparazione prima del primo lancio
                n.candidato = bid;
                n.pronto_da = tempo_ + REAZIONE_MISSILI_IA_S;
            }
            if (tempo_ < n.pronto_da) continue;
            const auto* t = trova(bid);
            const int salva = lc && lc->salva > 0 ? lc->salva : SALVA_SU_NAVE;
            n.salva = t && t->categoria == "sottomarino" ? SALVA_SU_SOTTOMARINO : salva;
            if (!dal_carico) n.salva = std::min(n.salva, n.rimasti);
            n.bersaglio = bid;
        }
        // il sottomarino: alla quota e alla velocita' di lancio, se lo script o la difesa non comandano adesso
        if (sub) {
            const bool pronto = u.quota <= d->quota_lancio_max_m && u.quota >= d->quota_lancio_min_m && std::fabs(u.velocita) <= d->velocita_lancio_max_kn;
            if (!pronto) {
                const auto os = ordini_script_.find(id);
                if ((os != ordini_script_.end() && os->second >= tempo_) || manovre_difesa_.count(id)) continue;
                if (!n.in_manovra) {
                    n.in_manovra = true;
                    n.quota_prima = u.quota_ordinata;
                    n.velocita_prima = u.velocita_ordinata;
                    n.manovra_da = tempo_;
                } else if (tempo_ - n.manovra_da > MANOVRA_LANCIO_MAX_S) {   // non ci arriva: rinuncia, e riprova piu' tardi
                    fine_manovra(u, n);
                    n.salva = 0;
                    n.prossimo = tempo_ + DOPO_SALVA_S;
                    continue;
                }
                u.quota_ordinata = 0.5 * (d->quota_lancio_min_m + d->quota_lancio_max_m);
                u.velocita_ordinata = std::min(std::fabs(u.velocita_ordinata), FRAZIONE_VELOCITA_LANCIO * d->velocita_lancio_max_kn);
                continue;
            }
        }
        // il piano dello stesso pianifica del giocatore, sul contatto (o sul quadro radio); fuori portata: niente lancio
        TiroTubo tt;
        if (radio) tt.fonte = FonteDati::radio;
        const int prima = u.bersaglio;
        u.bersaglio = bid;
        bool lanciato = false;
        if (const auto bs = bersaglio_missile(u, tt)) {
            if (!bs->distanza_nota) tt.imp.modo = ModoLancio::bol;
            const PianoTiro p = pianifica(*d, tt.imp, *bs, lanciatore_missile(u), contatti_noti_missile(u), tempo_);
            // con la scatola sporca l'IA non tira, e riprova (diag delle originali, 30/10/2026: in SCENE8A l'Harpoon d'una
            // Perry sull'Alfa agganciava un peschereccio del traffico)
            const bool pulita = scatola_pulita(p, bid, [&](int c) { const auto* x = trova(c); return x ? lato_creduto(u, *x) : 1; });
            if (p.valido && pulita) {
                crea_missile(u, *d, tt.imp, p, bs->contatto, 0);
                lanciato = true;
            }
        }
        u.bersaglio = prima;
        if (!lanciato) {   // il sottomarino resta alla quota di lancio fino al limite della manovra (niente su e giu')
            n.salva = 0;
            n.prossimo = tempo_ + RIPROVA_MISSILI_IA_S;
            if (!sub || (n.in_manovra && tempo_ - n.manovra_da > MANOVRA_LANCIO_MAX_S)) {
                fine_manovra(u, n);
                if (sub) n.prossimo = tempo_ + DOPO_SALVA_S;
            }
            continue;
        }
        if (dal_carico) {   // il tubo si vuota e si ricarica dalla riserva come per i siluri (passo_armi_ia)
            const int k = tubo_missile(id);
            auto& bt = A.battelli[id];
            bt.ultima[k] = bt.carico.tubi[static_cast<std::size_t>(k)];
            bt.carico.tubi[static_cast<std::size_t>(k)].clear();
            bt.ricarica[k] = tempo_ + RICARICA_TUBO_S;
        } else {
            --n.rimasti;
        }
        n.prossimo = --n.salva > 0 ? tempo_ + FRA_MISSILI_SALVA_S : tempo_;
    }
}

// Il TLAM dal tubo: la missione scelta, l'esito con la posizione vera e l'errore vero di navigazione (il pannello lo
// calcola sulla stima: un equipaggio «sicuro e sbagliato» vede VALIDA e manca il landfall). Falso senza una missione
bool Missione::lancia_tlam(UnitaClassica& g, int tubo) {
    const DatiMissile* d = missile_del_tubo(tubo);
    if (!d) return false;
    TiroTubo& t = tiri_[tubo];
    if (t.missione_tlam < 0 || t.missione_tlam >= static_cast<int>(libreria_tlam_.size())) return false;
    const MissioneTlam& mt = libreria_tlam_[static_cast<std::size_t>(t.missione_tlam)];
    LancioTlam x;
    x.t_lancio = tempo_;
    x.tubo = tubo;
    x.missione = mt.id;
    x.pannello = valuta_tlam(*d, mt, posizione_stimata(g.id), sigma_navigazione_m(g.id).value_or(0.0), tempo_);
    const Vec2 e = errore_navigazione(g.id);
    x.vero = valuta_tlam(*d, mt, g.pos, std::hypot(e.x, e.y), tempo_);
    x.esito = esito_tlam(*d, mt, x.vero, tempo_, seme_ ^ (0x71A3ull + static_cast<std::uint64_t>(lanci_tlam_.size()) * 0x9E37ull));
    lanci_tlam_.push_back(x);
    LancioMissile v;   // per gli effetti del lancio (partita.cpp): verso il landfall
    v.tempo = tempo_;
    v.unita = g.id;
    v.pos = g.pos;
    v.quota = g.quota;
    v.rotta = normalizza_360(rilevamento(g.pos, mt.landfall));
    v.classe_lanciatore = g.classe;
    v.categoria_lanciatore = g.categoria;
    v.arma = d->id;
    lanci_missili_.push_back(v);
    const auto lato = static_cast<std::size_t>(lato_tubo(tubo));
    slide_dal_[lato] = tempo_ + SLIDE_STESSO_LATO_S;
    slide_dal_[1 - lato] = std::max(slide_dal_[1 - lato], tempo_ + SLIDE_ALTRO_LATO_S);
    t.prep.reset();
    t.missile.clear();
    tubi_usati_.insert(tubo);
    return true;
}

// il giocatore sa dell'aggancio se vede il bersaglio (la testa del periscopio fuori, entro l'orizzonte del periscopio
// contro la sovrastruttura) o se glielo riferisce un'unita' amica che ha il bersaglio in contatto (STIMA: nel 1989 il
// missile non ha collegamento col battello)
std::optional<ImpostazioniMissile> Missione::impostazioni_missile(int arma) const {
    const auto it = missili_.find(arma);
    if (it == missili_.end()) return std::nullopt;
    return it->second.imp;
}

bool Missione::aggancio_visto(int nave) const {
    const auto* g = giocatore();
    const auto ib = unita_.find(nave);
    if (!g || ib == unita_.end()) return false;
    if (const auto testa = testa_giocatore(); testa && distanza(g->pos, ib->second.pos) <= orizzonte_radar_m(testa->altezza_m, ALTEZZA_BERSAGLIO_M))
        return true;
    for (const auto& [id, u] : unita_) {
        if (u.giocatore || u.distrutta || u.persa || (u.flag & 0x20) || u.categoria == "arma" || u.categoria == "boa" || lato_di(u) != lato_di(*g)) continue;
        if (u.categoria == "sottomarino" && u.quota > 20.0) continue;   // in immersione non trasmette
        if (const auto c = u.contatti.find(nave); c != u.contatti.end() && c->second.visibile()) return true;
    }
    return false;
}

void Missione::passo_missili(double dt) {
    // i TLAM all'ora d'arrivo: la verita' nel registro; al giocatore il BDA alla radio, piu' tardi (nel 1989 il battello non
    // ha collegamento col missile: lo sa dal broadcast)
    for (auto& x : lanci_tlam_) {
        if (x.riferito || tempo_ < x.esito.t_arrivo) continue;
        x.riferito = true;
        const MissioneTlam* mt = nullptr;
        for (const auto& t : libreria_tlam_) if (t.id == x.missione) mt = &t;
        const std::string bersaglio = mt ? mt->bersaglio : std::string{};
        const bool in_finestra = mt && std::fabs(x.esito.t_arrivo - mt->tot_s) <= mt->finestra_s;   // I12: sul bersaglio all'ora
        registro_.esito_tlam(*this, x.missione, bersaglio, x.esito.arrivato, x.esito.t_arrivo - x.t_lancio,
                             x.esito.arrivato && mt ? mt->landfall : x.esito.caduta, in_finestra);
        if (radio_) {
            MessaggioRadio m;
            m.precedenza = Precedenza::priority;
            m.da = "COMSUBLANT";
            m.oggetto = "BDA";
            m.classifica = "SECRET";
            m.testo = x.esito.arrivato
                          ? TestoRadio{"BDA MISSIONE TLAM " + x.missione + ": BERSAGLIO " + bersaglio + " COLPITO.",
                                       "BDA TLAM MISSION " + x.missione + ": TARGET " + bersaglio + " HIT."}
                          : TestoRadio{"BDA MISSIONE TLAM " + x.missione + ": NESSUN EFFETTO OSSERVATO SU " + bersaglio + ".",
                                       "BDA TLAM MISSION " + x.missione + ": NO EFFECT OBSERVED ON " + bersaglio + "."};
            radio_->programma(m, tempo_ + BDA_TLAM_S);
        }
    }
    for (auto& [tubo, t] : tiri_) {
        if (!t.prep) continue;
        const DatiMissile* d = missile_del_tubo(tubo);
        if (!d || d->id != t.missile) { t.prep.reset(); t.missile.clear(); continue; }   // il tubo ha cambiato arma
        t.prep->avanza(dt);
    }
    if (missili_.empty()) return;
    // la verita' delle navi in superficie, coi radar accesi dell'ultimo passo dell'elettronica
    const auto eco = [&](const UnitaClassica& u) {
        EcoNave n;
        n.id = u.id;
        n.pos = u.pos;
        n.rotta = u.rotta;
        n.velocita_m_s = std::fabs(u.velocita) * KN;
        n.dimensione = dimensione_creduta(u.classe);
        for (const auto& [rid, radar] : radar_del_passo_)
            if (rid == u.id) n.radar_accesi.push_back(radar);
        return n;
    };
    // le navi e i sottomarini in superficie; non le armi, gli aerei
    const auto in_superficie = [](const UnitaClassica& u) {
        const bool superficie = u.categoria == "nave" || (u.categoria == "sottomarino" && u.quota <= 2.0);
        return !u.distrutta && !u.persa && superficie && !(u.flag & 0x20) && (u.flag & 3) != 3;
    };
    std::vector<EcoNave> navi;
    for (const auto& [id, u] : unita_)
        if (in_superficie(u) && !(u.flag & 0x80)) navi.push_back(eco(u));   // non i falsi bersagli
    const int mare = stato_mare();
    for (auto it = missili_.begin(); it != missili_.end();) {
        MissileMissione& x = it->second;
        UnitaClassica* u = trova(it->first);
        if (!u || u->distrutta || u->persa) { it = missili_.erase(it); continue; }
        // la testa non prende il proprio lanciatore, se e' fra gli echi (a galla, nel poco fondo o dopo il lancio di una nave)
        const auto del_lanciatore = [&](const EcoNave& n) { return n.id == x.missile.lanciatore(); };
        // il falso bersaglio designato e' un eco per il suo missile: e' li' per quello (SCENE0A: il bersaglio radar che i TASM
        // sorvolavano fino al traghetto)
        const auto* designato = trova(x.piano.contatto);
        const bool falso_designato = designato && (designato->flag & 0x80) && in_superficie(*designato);
        if (falso_designato || std::any_of(navi.begin(), navi.end(), del_lanciatore)) {
            std::vector<EcoNave> altre = navi;
            altre.erase(std::remove_if(altre.begin(), altre.end(), del_lanciatore), altre.end());
            if (falso_designato) altre.push_back(eco(*designato));
            x.missile.avanza(dt, altre, mare);
        } else {
            x.missile.avanza(dt, navi, mare);
        }
        u->pos = x.missile.pos();
        u->rotta = u->rotta_ordinata = x.missile.rotta();
        u->quota = u->quota_ordinata = 0.0;   // in aria: la quota della missione e' la profondita'
        u->velocita = u->velocita_ordinata = x.missile.dati().velocita_m_s / KN;
        u->bersaglio = x.missile.bersaglio() >= 0 ? x.missile.bersaglio() : u->bersaglio;
        for (const auto& e : x.missile.eventi()) {
            if (e.tipo == EventoMissile::Tipo::aggancio && x.giocatore && !x.aggancio_detto && aggancio_visto(e.nave)) {
                x.aggancio_detto = true;
                messaggio_di_bordo(x.missile.lanciatore(), MSG_MISSILE_AGGANCIA);
            }
            if (e.tipo == EventoMissile::Tipo::esaurito || e.tipo == EventoMissile::Tipo::guasto)
                registro_.fine_missile(*this, x.missile.lanciatore(), it->first, u->classe, e.tipo == EventoMissile::Tipo::guasto,
                                       x.missile.tempo_s(), x.missile.pos());
            if (e.tipo != EventoMissile::Tipo::impatto) continue;
            UnitaClassica* v = trova(e.nave);
            if (!v || v->distrutta || v->persa) continue;
            // le difese (nucleo/difese_navi): il byte della nave, scalato con l'allerta e fino ai canali, poi il Phalanx. Solo le
            // navi in tabella: per le altre e per chi non e' una nave, il byte intero come l'ATTACK_RND. Poi il danno della testata
            const bool nave = v->categoria == "nave";
            const DifesaClasse* difesa = nave ? catalogo_missili_->difesa(v->classe) : nullptr;
            const double fattore = difesa ? fattore_allerta(allerta_contro_missile(*v, x.forze_al_lancio)) : 1.0;
            const EsitoDifesa esito = impegno_difese_[v->id].arrivo(difesa, v->difesa, fattore, tempo_, x.missile.dati().velocita_m_s, anno_, casuale());
            if (esito != EsitoDifesa::colpo) {   // fermato: lo scoppio in aria prima della nave, lungo la sua rotta (T13 del kit)
                if (nave && !arma_esercitazione(it->first)) evento_allerta(v->id, "arma");   // il prossimo missile la trova sveglia
                const DatiMissile& dm = x.missile.dati();
                const double a = radianti(x.missile.rotta());
                const Vec2 p{e.pos.x - ABBATTUTO_PRIMA_M * std::sin(a), e.pos.y - ABBATTUTO_PRIMA_M * std::cos(a)};
                const double kg = arma_esercitazione(it->first) ? 0.0 : dm.esplosivo_kg > 0.0 ? dm.esplosivo_kg : dm.testata_kg;
                registro_.missile_abbattuto(*this, x.missile.lanciatore(), it->first, u->classe, v->id, p,
                                            std::max(x.missile.quota(), QUOTA_ABBATTUTO_MIN_M), kg);
                continue;
            }
            if (arma_esercitazione(it->first)) {   // inerte: il colpo d'esercitazione (obiettivo `colpire`), nessun danno
                colpi_esercitazione_.push_back({tempo_, it->first, x.missile.lanciatore(), v->id, u->classe, v->pos});
                const auto* g = giocatore();
                if (g && x.missile.lanciatore() == g->id) messaggio(g->id, MSG_ESERCITAZIONE_A_SEGNO);
                if (g && v->id == g->id) messaggio(g->id, MSG_ESERCITAZIONE_COLPITI);
                continue;
            }
            if (x.giocatore) messaggio(x.missile.lanciatore(), MSG_MISSILE_A_SEGNO);
            if (v->affondamento == 0) v->colpo = 0xC9;
            // il danno sull'esplosivo, l'Harpoon di riferimento (sulla stessa scala dei depositi dei danni critici, il
            // kit); senza l'esplosivo nei dati, il rapporto delle testate di prima
            const DatiMissile& dm = x.missile.dati();
            const double danno = dm.esplosivo_kg > 0.0 ? DANNO_HARPOON * dm.esplosivo_kg / ESPLOSIVO_HARPOON_KG
                                                       : DANNO_HARPOON * dm.testata_kg / TESTATA_HARPOON_KG;
            // il colpo e' di chi ha lanciato questo missile: il +62 della vittima (attacco_casuale) resta com'era per gli script
            const int lanciatore_prima = v->lanciatore;
            const std::size_t colpi_prima = colpi_.size();
            v->lanciatore = x.missile.lanciatore();
            applica_attacco(*v, static_cast<int>(std::lround(std::max(30.0, danno))), Colpo::Tipo::attacco_casuale);
            v->lanciatore = lanciatore_prima;
            if (colpi_.size() > colpi_prima) {
                colpi_.back().arma = it->first;
                colpi_.back().classe_arma = u->classe;
            }
        }
        if (!x.missile.attivo()) {
            togli(*u);
            it = missili_.erase(it);
            continue;
        }
        ++it;
    }
}

}  // namespace m688
