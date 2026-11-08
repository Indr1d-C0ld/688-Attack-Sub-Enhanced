// Periscopi dell'Enhanced nella missione (docs/14_periscopi.md, P2): il posto del giocatore (nucleo/periscopi), la testa
// del giocatore vista da navi, aerei e periscopi nemici, le guardate dell'IA, foto ed esposizioni per il registro M1.

#include <algorithm>
#include <cmath>

#include "missioni/periscopi_missione.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"
#include "nucleo/battello.hpp"
#include "nucleo/sole.hpp"

namespace m688 {

namespace {

constexpr double OCCHIO_NAVE_M = 20.0;         // vedette sul ponte (STIMA)
constexpr int MSG_ALBERI_GHIACCIO = 489;       // «ghiaccio sopra: alberi e periscopio non possono salire» (missione_ghiaccio.cpp)
constexpr double PERDITA_VELOCITA = 0.6;       // il periscopio piegato dalla velocita': resta sotto 0,5
constexpr double CANNA_ALLAGATA_M3_S = 0.005;  // la canna del periscopio cieco lasciata su: ~18 m^3 all'ora nella centrale (STIMA)
constexpr double PORTATA_FOTO_M = 25000.0;
// il periscopio dell'IA che guarda vede le navi entro la portata a vista (portata_vista: la sagoma con la luce, la
// foschia e l'orizzonte; di notte anche i fanali): un contatto con la posizione (docs/18 fase 2)
constexpr double COPERTO_LUCE = 0.5;           // il cielo coperto (8 ottavi) dimezza la luce del giorno (STIMA)
constexpr double QUOTA_TESTA_SCRIPT_M = 1.5;   // lo script alza il periscopio a 20 ft: testa fuori fino a 20 ft + 1,5 m

bool stesso_lato(const UnitaClassica& a, const UnitaClassica& b) { return ((a.flag ^ b.flag) & 0x10) == 0; }
bool arma(const UnitaClassica& u) { return (u.flag & 0x20) != 0 || u.categoria == "arma"; }

double quota_classe(const DatiClassePeriscopi& c, double quota_missione) {
    return quota_missione - QUOTA_PERISCOPICA_MISSIONE_M + c.quota_m;
}

}  // namespace

void Missione::periscopi_enhanced(const std::string& cartella_dati) {
    if (peri_ || peri_tentato_) return;
    const auto* g = giocatore();
    if (!g) return;
    peri_tentato_ = true;
    auto p = std::make_unique<StatoPeriscopiMissione>();
    try {
        p->dati = carica_periscopi(cartella_dati + "/periscopi.toml");
    } catch (const std::exception&) {
        ordini_peri_in_attesa_.clear();
        return;   // senza i dati resta il periscopio dell'originale
    }
    p->posto = PostoPeriscopi(p->dati, g->classe);
    p->caso = Casuale(seme_ ^ 0x5C09E5ull);
    p->prossimo = tempo_;
    // un ordine su/giu' del secondo 0 l'ha eseguito il classico: il flag torna giu' e il posto lo rifa' coi suoi secondi
    const bool ordinato = std::any_of(ordini_peri_in_attesa_.begin(), ordini_peri_in_attesa_.end(),
                                      [](const Ordine& o) { return o.tipo == TipoOrdine::periscopio; });
    if (ordinato) unita_.at(g->id).flag54 &= ~2;
    // il periscopio gia' alzato dalla missione: il posto parte su
    else if (g->flag54 & 2) {
        p->posto.comanda(true);
        for (int i = 0; i < 200; ++i) p->posto.avanza(tempo_, 0.1, quota_classe(p->posto.classe(), g->quota), 0.0, stato_mare(), p->caso, meteo_qui_.hs_m());
        p->flag_nostro = true;
    }
    peri_ = std::move(p);
    for (const Ordine& o : ordini_peri_in_attesa_) ordine_periscopio(o);   // quelli arrivati prima, nell'ordine
    ordini_peri_in_attesa_.clear();
}

const PostoPeriscopi* Missione::posto_periscopi() const { return peri_ ? &peri_->posto : nullptr; }

void Missione::vista_periscopio(int ingrandimento, ModoVista modo, int filtro, double sdoppiamento_gradi,
                                bool telecamera_verticale) {
    if (!peri_) return;
    peri_->posto.ingrandimento(ingrandimento);
    peri_->posto.modo(modo);
    peri_->posto.filtro(filtro);
    peri_->posto.sdoppiamento(sdoppiamento_gradi);
    peri_->posto.telecamera_verticale(telecamera_verticale);
}

double Missione::luce_periscopi() const { return peri_ ? peri_->luce : 1.0; }

bool Missione::testa_ia_fuori(int id) const {
    if (!peri_) return false;
    const auto it = peri_->ia.find(id);
    return it != peri_->ia.end() && it->second.esposto();
}

std::vector<TestaIa> Missione::teste_ia() const {
    std::vector<TestaIa> v;
    if (!peri_) return v;
    const auto* g = giocatore();
    for (const auto& [id, u] : unita_) {
        if (u.distrutta || u.persa || (g && id == g->id) || u.categoria != "sottomarino") continue;
        const auto it = peri_->ia.find(id);
        const DatiClassePeriscopi* c = peri_->dati.classe(u.classe);
        if (it != peri_->ia.end() && it->second.esposto() && c) {
            v.push_back({id, it->second.modello() ? it->second.modello()->id : "", u.pos,
                         it->second.altezza_testa_m(quota_classe(*c, u.quota)), false});
        } else if ((u.flag54 & 2) && u.quota <= QUOTA_PERISCOPICA_MISSIONE_M + QUOTA_TESTA_SCRIPT_M) {
            const auto modelli = peri_->dati.di_classe(u.classe);
            const double h = c ? altezza_testa_m(*c, quota_classe(*c, u.quota), 1.0)
                               : QUOTA_PERISCOPICA_MISSIONE_M + QUOTA_TESTA_SCRIPT_M - u.quota;
            if (h > 0.0) v.push_back({id, modelli.empty() ? "" : modelli.front()->id, u.pos, h, true});
        }
    }
    return v;
}

std::optional<TestaEsposta> Missione::testa_giocatore() const {
    if (!peri_ || !peri_->posto.stato().testa_fuori || peri_->posto.stato().finestra_coperta) return std::nullopt;
    const auto* g = giocatore();
    const SagomaTesta s = peri_->posto.sagoma(g ? g->velocita : 0.0);
    return TestaEsposta{s.sezione_m2, s.altezza_m, s.piuma};
}

bool Missione::esm_in_testa() const {
    return peri_ && peri_->posto.stato().testa_fuori && peri_->posto.dati()->esm;
}

// --- ordini ---

bool Missione::ordine_periscopio(const Ordine& o) {
    auto& P = *peri_;
    const auto* g = giocatore();
    if (!g) return false;
    switch (o.tipo) {
        case TipoOrdine::periscopio:
            P.posto.comanda(o.n != 0);
            if (o.n != 0 && P.posto.stato().danneggiato) messaggi_peri_.push_back({tempo_, g->id, MSG_PERI_NON_SALE});
            return true;
        case TipoOrdine::scegli_periscopio:
            if (!P.posto.scegli(o.n)) return false;
            messaggi_peri_.push_back({tempo_, g->id, MSG_PERI_SCELTO});
            return true;
        case TipoOrdine::elevazione_periscopio:
            P.posto.elevazione(o.a);
            return true;
        case TipoOrdine::foto_periscopio: {
            const auto& s = P.posto.stato();
            const DatiPeriscopio& d = *P.posto.dati();
            if (!s.su || !d.foto) return false;
            FotoPeriscopio f;
            f.tempo = tempo_;
            f.rilevamento = normalizza_360(o.a);
            f.ingrandimento = std::clamp(o.n, 0, static_cast<int>(d.ingrandimenti.size()) - 1);
            f.campo_gradi = d.campi_gradi[static_cast<std::size_t>(f.ingrandimento)];
            f.periscopio = d.id;
            std::vector<std::pair<double, int>> vicini;
            for (const auto& [id, u] : unita_) {
                if (id == g->id || u.distrutta || u.persa) continue;
                const bool sopra = aereo(u) || u.quota <= QUOTA_PERISCOPICA_MISSIONE_M + QUOTA_TESTA_SCRIPT_M;
                const double dist = distanza(g->pos, u.pos);
                if (!sopra || dist > PORTATA_FOTO_M) continue;
                if (std::fabs(normalizza_180(rilevamento(g->pos, u.pos) - f.rilevamento)) > f.campo_gradi / 2.0) continue;
                vicini.push_back({dist, id});
            }
            std::sort(vicini.begin(), vicini.end());
            for (const auto& [dist, id] : vicini) f.in_vista.push_back(id);
            foto_.push_back(std::move(f));
            messaggi_peri_.push_back({tempo_, g->id, MSG_PERI_FOTO});
            return true;
        }
        default: return false;
    }
}

// --- danni: il sistema «periscopio» della fisica dei battelli ---

void Missione::danni_periscopio(const DanniSottomarini* sub) {
    const auto* g = giocatore();
    if (!sub || !g) return;
    const auto* s = sub->sistemi(g->id);
    const bool avaria = s && static_cast<std::size_t>(Sistema::periscopio) < s->size() &&
                        (*s)[static_cast<std::size_t>(Sistema::periscopio)].efficienza < 0.5;
    if (avaria && !peri_->avaria) peri_->posto.danneggia();
    if (!avaria && peri_->avaria) peri_->posto.ripara();
    peri_->avaria = avaria;
    // le teste colpite dall'artiglieria (G3 del kit, dossier 22 §5.3): bloccate, cieche (con la canna che si allaga),
    // degradate
    if (const DanniArtiglieriaBattello* a = sub->artiglieria(g->id)) {
        int k = 0;
        for (const auto& [testa, ruolo] : {std::pair<const TestaColpita*, const char*>{&a->attacco, "attacco"}, {&a->ricerca, "ricerca"}}) {
            const int i = peri_->posto.indice_ruolo(ruolo);
            double& visto = peri_->teste_tempo[k++];
            if (i < 0 || testa->tempo < 0.0 || testa->tempo == visto) continue;
            visto = testa->tempo;
            using D = PostoPeriscopi::DannoTesta;
            const D d = testa->guasto == GuastoTesta::bloccata ? D::bloccata : testa->guasto == GuastoTesta::cieca ? D::cieca
                      : testa->guasto == GuastoTesta::degradata ? D::degradata : D::nessuno;
            peri_->posto.danneggia_testa(i, d, testa->allaga_canna);
        }
    }
}

double Missione::allagamento_periscopio_m3_s(int unita) const {
    const auto* g = giocatore();
    if (!peri_ || !g || g->id != unita || !peri_->posto.canna_aperta()) return 0.0;
    return CANNA_ALLAGATA_M3_S;
}

// --- un passo ---

void Missione::passo_periscopi(double dt) {
    auto& P = *peri_;
    const auto* gc = giocatore();
    if (!gc || gc->distrutta || gc->persa) return;
    UnitaClassica& g = unita_.at(gc->id);
    // il flag abbassato da fuori (sotto i 20 ft si abbassa da solo, controlli_giocatore): anche l'ordine torna giu'
    if (P.flag_nostro && !(g.flag54 & 2)) {
        P.posto.comanda(false);
        P.flag_nostro = false;
    }
    // un comando a un periscopio bloccato dall'artiglieria: non sale e non scende (avviso 404, una volta per comando)
    if (P.posto.bloccato()) {
        const bool cmd = P.posto.stato().comandato_su;
        if (cmd != P.comando_bloccato) {
            P.comando_bloccato = cmd;
            messaggi_peri_.push_back({tempo_, g.id, MSG_PERI_NON_SALE});
        }
    } else {
        P.comando_bloccato = P.posto.stato().comandato_su;
    }
    // sotto la banchisa (missione_ghiaccio.cpp) il periscopio non sale: l'ordine torna giu', l'avviso 489 a ogni comando
    if (P.posto.stato().comandato_su && sotto_banchisa(g.id)) {
        P.posto.comanda(false);
        messaggio_di_bordo(g.id, MSG_ALBERI_GHIACCIO);
    }
    const bool era_su = P.posto.stato().su;
    if (P.posto.avanza(tempo_, dt, quota_classe(P.posto.classe(), g.quota), g.velocita, stato_mare(), P.caso, meteo_qui_.hs_m())) {
        guasti_.push_back({g.id, static_cast<int>(Sistema::periscopio), PERDITA_VELOCITA, "Periscopio danneggiato dalla velocità"});
        messaggi_peri_.push_back({tempo_, g.id, MSG_PERI_DANNEGGIATO});
    }
    const StatoPeriscopio& s = P.posto.stato();
    // il flag dell'originale a fine corsa (il gestore dello script), giu' quando la testa torna sotto
    if (s.su && s.comandato_su && !(g.flag54 & 2)) {
        comando("periscopio", {1.0});
        P.flag_nostro = (g.flag54 & 2) != 0;
        if (!era_su) messaggi_peri_.push_back({tempo_, g.id, MSG_PERI_SU});
    } else if (!s.su && P.flag_nostro && (g.flag54 & 2)) {
        comando("periscopio", {0.0});
        P.flag_nostro = false;
    }
    // avvisi: immagine inservibile, esposti da 30 s
    if (s.testa_fuori && s.tremolio >= 1.0 && !P.inservibile_avvisato) {
        messaggi_peri_.push_back({tempo_, g.id, MSG_PERI_INSERVIBILE});
        P.inservibile_avvisato = true;
    } else if (s.tremolio < 0.5) {
        P.inservibile_avvisato = false;
    }
    if (s.esposto_s >= ESPOSIZIONE_LUNGA_S && !P.esposti_avvisato) {
        messaggi_peri_.push_back({tempo_, g.id, MSG_PERI_ESPOSTI});
        P.esposti_avvisato = true;
    } else if (!s.testa_fuori) {
        P.esposti_avvisato = false;
    }
    for (const auto& e : P.posto.finite())
        if (e.durata() > ESPOSIZIONE_LUNGA_S) esposizioni_lunghe_.push_back(e);

    // un secondo di vista
    if (tempo_ < P.prossimo) return;
    P.prossimo = tempo_ + 1.0;
    const double passo = 1.0;
    if (tempo_ - P.ultima_luce >= 60.0) {
        const double ore = std::fmod(ora_utc_s_ + tempo_, 86400.0) / 3600.0;
        const double lat = luogo_sole_ ? luogo_sole_->first : latitudine_, lon = luogo_sole_ ? luogo_sole_->second : longitudine_;
        // il coperto toglie luce di giorno: col cielo coperto meta' (STIMA, docs E8); di notte non cambia niente
        const double coperto = meteo_ ? std::clamp(meteo_in(g.pos).nubi_ottavi / 8.0, 0.0, 1.0) : 0.0;
        P.luce = luce_ambiente(elevazione_sole({anno_, mese_data_, giorno_, ore}, lat, lon)) * (1.0 - COPERTO_LUCE * coperto);
        P.ultima_luce = tempo_;
    }
    // le guardate dell'IA
    for (const auto& [id, u] : unita_) {
        if (u.distrutta || u.persa || id == g.id || u.categoria != "sottomarino") continue;
        if (!P.considerati.count(id)) {
            P.considerati.insert(id);
            if (P.dati.classe(u.classe)) P.ia.emplace(id, PeriscopioIa(P.dati, u.classe, tempo_, P.caso));
        }
        const auto it = P.ia.find(id);
        if (it == P.ia.end()) continue;
        bool allarme = false, radar = false;
        for (const auto& [k, v] : unita_)
            if (!v.distrutta && !v.persa && arma(v) && v.bersaglio == id) { allarme = true; break; }
        if (it->second.fase() == FaseGuardata::avvisatore && catalogo_el_)
            for (const auto& [k, e] : emcon_) {
                const auto v = unita_.find(k);
                if (v == unita_.end() || v->second.distrutta || stesso_lato(v->second, u) || distanza(v->second.pos, u.pos) > 40000.0) continue;
                for (const Radar* r : catalogo_el_->radar_di(v->second.classe)) radar = radar || e.emette(*r);
            }
        // sotto la banchisa niente guardate (come in allarme)
        it->second.avanza(tempo_, quota_classe(*P.dati.classe(u.classe), u.quota), u.velocita, allarme || sotto_banchisa(id), radar, P.luce, P.caso);
        if (!it->second.esposto()) P.nemici_visti.erase(id);
    }
    // la testa del giocatore vista da navi, aerei e periscopi dell'altra parte
    if (s.testa_fuori && !s.finestra_coperta) {   // l'onda sulla finestra: in quell'istante nessuno vede
        const DatiPeriscopio& d = *P.posto.dati();
        const SagomaTesta testa = P.posto.sagoma(g.velocita);
        CondizioniVista c;
        c.stato_mare = stato_mare();   // il meteo nel punto del giocatore (M2)
        c.luce = P.luce;
        c.visibilita_m = meteo_qui_.visibilita_m;
        c.nubi_ottavi = meteo_qui_.nubi_ottavi;
        c.nebbia_radiazione = meteo_qui_.nebbia == Nebbia::radiazione;
        for (auto& [id, u] : unita_) {
            if (id == g.id || u.distrutta || u.persa || arma(u) || stesso_lato(u, g)) continue;
            Osservatore o;
            double fattore = 1.0;   // la TV МТ-70 vede piu' corto
            if (aereo(u)) {
                o.tipo = TipoOsservatore::aereo;
                o.altezza_occhio_m = quota_aerea(u);
            } else if (u.categoria == "nave") {
                o.altezza_occhio_m = OCCHIO_NAVE_M;
            } else if (const auto it = P.ia.find(id); it != P.ia.end() && it->second.fase() != FaseGuardata::sotto &&
                                                      it->second.fase() != FaseGuardata::avvisatore) {
                o = it->second.osservatore(quota_classe(*P.dati.classe(u.classe), u.quota));
                fattore = it->second.fattore_tv();
            } else {
                continue;
            }
            const auto ci = u.contatti.find(g.id);
            o.allertato = ci != u.contatti.end();
            const double p = probabilita_vista(d, testa, o, c, distanza(u.pos, g.pos) / fattore, passo);
            if (p > 0.0) possibile(id, g.id, "vista", 0.0);
            if (p <= 0.0 || P.caso.uniforme() >= p) continue;
            Contatto* k = ci != u.contatti.end() ? &ci->second : nuovo_contatto(u, g.id);
            if (!k) continue;
            k->qualita = std::max(k->qualita, 800.0);
            k->lampeggio = 0;
            k->ultima_scoperta = tempo_;
            k->rilevamento = rilevamento(u.pos, g.pos);
            k->pos_stimata = g.pos;
            k->quota_stimata = g.quota;
            k->posizione_nota = true;
            k->t_posizione = tempo_;
            k->sensore = "vista";
            if (const auto e = emcon_.find(id); e != emcon_.end()) e->second.evento(EventoEmcon::albero_visto, tempo_);
            evento_allerta(id, "albero");   // fase 3: periscopio o albero visto
            scelta_bersaglio(u, g, *k);
        }
        // le teste dell'IA viste dal periscopio del giocatore
        Osservatore noi;
        noi.tipo = TipoOsservatore::periscopio;
        noi.altezza_occhio_m = std::max(0.5, s.altezza_testa_m);
        for (const ModoVista m : d.modi)
            if (m != ModoVista::giorno && noi.notte == ModoVista::giorno) noi.notte = m;   // la vedetta usa la notte
        noi.allertato = true;
        for (const auto& t : teste_ia()) {
            auto& u = unita_.at(t.unita);
            if (stesso_lato(u, g)) continue;
            const DatiPeriscopio* m = P.dati.periscopio(t.periscopio);
            if (!m) continue;
            const double p = probabilita_vista(*m, sagoma_testa(*m, t.altezza_m, u.velocita), noi, c, distanza(u.pos, g.pos), passo);
            if (p > 0.0) possibile(g.id, t.unita, "vista", 0.0);
            if (p <= 0.0 || P.caso.uniforme() >= p) continue;
            Contatto* k = g.contatti.count(t.unita) ? &g.contatti.at(t.unita) : nuovo_contatto(g, t.unita);
            if (!k) continue;
            k->qualita = std::max(k->qualita, 300.0);
            k->lampeggio = 0;
            k->ultima_scoperta = tempo_;
            if (!k->posizione_nota || k->sensore == "vista") { k->rilevamento = rilevamento(g.pos, u.pos); k->sensore = "vista"; }
            if (P.nemici_visti.insert(t.unita).second) messaggi_peri_.push_back({tempo_, t.unita, MSG_PERI_NEMICO});
        }
    }
    // fase 2 delle interazioni (docs/18): le teste dell'IA viste dalle vedette e dai periscopi dell'IA dell'altra parte,
    // con la regola della testa del giocatore; e il periscopio dell'IA che guarda vede le navi
    for (const auto& t : teste_ia()) {
        auto& bu = unita_.at(t.unita);
        const DatiPeriscopio* m = P.dati.periscopio(t.periscopio);
        if (!m) continue;
        const SagomaTesta sagoma = sagoma_testa(*m, t.altezza_m, bu.velocita);
        const StatoMeteo qui = meteo_ascolto(t.pos);
        CondizioniVista c;
        c.stato_mare = qui.stato_mare();   // il mare nel punto della testa, come visibilita' e nubi
        c.luce = P.luce;
        c.visibilita_m = qui.visibilita_m;
        c.nubi_ottavi = qui.nubi_ottavi;
        c.nebbia_radiazione = qui.nebbia == Nebbia::radiazione;
        for (auto& [id, u] : unita_) {
            if (id == t.unita || u.giocatore || u.distrutta || u.persa || arma(u) || stesso_lato(u, bu)) continue;
            Osservatore o;
            double fattore = 1.0;
            if (aereo(u)) {
                o.tipo = TipoOsservatore::aereo;
                o.altezza_occhio_m = quota_aerea(u);
            } else if (u.categoria == "nave") {
                o.altezza_occhio_m = OCCHIO_NAVE_M;
            } else if (const auto it = P.ia.find(id); it != P.ia.end() && it->second.fase() != FaseGuardata::sotto &&
                                                      it->second.fase() != FaseGuardata::avvisatore) {
                o = it->second.osservatore(quota_classe(*P.dati.classe(u.classe), u.quota));
                fattore = it->second.fattore_tv();
            } else {
                continue;
            }
            const auto ci = u.contatti.find(t.unita);
            o.allertato = ci != u.contatti.end();
            const double p = probabilita_vista(*m, sagoma, o, c, distanza(u.pos, bu.pos) / fattore, passo);
            if (p > 0.0) possibile(id, t.unita, "vista", 0.0);
            if (p <= 0.0 || P.caso.uniforme() >= p) continue;
            Contatto* k = ci != u.contatti.end() ? &ci->second : nuovo_contatto(u, t.unita);
            if (!k) continue;
            k->qualita = std::max(k->qualita, 800.0);
            k->lampeggio = 0;
            k->ultima_scoperta = tempo_;
            k->rilevamento = rilevamento(u.pos, bu.pos);
            k->pos_stimata = bu.pos;
            k->quota_stimata = bu.quota;
            k->posizione_nota = true;
            k->t_posizione = tempo_;
            k->sensore = "vista";
            if (const auto e = emcon_.find(id); e != emcon_.end()) e->second.evento(EventoEmcon::albero_visto, tempo_);
            evento_allerta(id, "albero");   // fase 3: periscopio o albero visto
            scelta_bersaglio(u, bu, *k);
        }
    }
    for (auto& [id, u] : unita_) {   // il periscopio dell'IA fuori: le navi dell'altra parte entro la vista
        if (u.giocatore || u.distrutta || u.persa || u.categoria != "sottomarino") continue;
        const auto it = P.ia.find(id);
        if (it == P.ia.end() || !it->second.esposto()) continue;
        const Osservatore occhio = it->second.osservatore(quota_classe(*P.dati.classe(u.classe), u.quota));
        for (auto& [nid, n] : unita_) {
            if (n.categoria != "nave" || n.distrutta || n.persa || stesso_lato(u, n)) continue;
            // la sagoma a ogni luce, e di notte i fanali (docs/27; missione_luci.cpp)
            if (distanza(u.pos, n.pos) > portata_vista(nid, u.pos, occhio)) continue;
            possibile(id, nid, "vista", 0.0);
            Contatto* k = u.contatti.count(nid) ? &u.contatti.at(nid) : nuovo_contatto(u, nid);
            if (!k) continue;
            k->qualita = std::max(k->qualita, 800.0);
            k->lampeggio = 0;
            k->ultima_scoperta = tempo_;
            k->rilevamento = rilevamento(u.pos, n.pos);
            k->pos_stimata = n.pos;
            k->quota_stimata = 0.0;
            k->posizione_nota = true;
            k->t_posizione = tempo_;
            k->sensore = "vista";
            scelta_bersaglio(u, n, *k);
        }
    }
    vedetta_luci(dt);   // docs/27: il giocatore illuminato visto di notte (missione_luci.cpp)
}

}  // namespace m688
