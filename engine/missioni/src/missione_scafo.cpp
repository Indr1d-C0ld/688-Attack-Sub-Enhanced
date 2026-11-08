// I transitori dello scafo (nota condivisa disassemblatore-transitori-scafo.md; voci [transitori.*] di acustica.toml con
// categoria "scafo" e densita' 0): il motore li fa nascere dagli stati della missione, per tutti i sottomarini immersi,
// giocatore e IA (la simmetria). Vanno nella scena acustica come i transitori delle armi (transitori_armi_, fascia di id
// 950000-959999); l'IA li sente coi suoi sensori passivi (ascolta_transitorio) e ne ricava «sottomarino». Le frequenze
// (STIMA) dipendono dall'equipaggio (x3 recluta, x1 medio, x0,4 esperto) e dal silenzio di manovra o dai posti di
// combattimento (x0,25); le estrazioni da un Casuale del battello, deterministiche.
// E i posti di combattimento del battello del giocatore (l'allarme sonoro di trailer-1).

#include <algorithm>
#include <cmath>

#include "missioni/missione.hpp"

namespace m688 {

namespace {

constexpr int SCAFO_DA = 950000, SCAFO_A = 959999;
constexpr double IMMERSO_M = 3.0;                       // sotto, lo scafo si sente in acqua
constexpr double PORTELLO_H = 0.5, ATTREZZO_H = 0.3;    // a caso, per ora di missione
constexpr double P_SCOSSONE = 0.3, SCOSSONE_M = 2000.0; // l'attrezzo che cade dopo un urto o un'esplosione vicina
constexpr double PIANI_INVERSIONE_M_S = 1.0, PIANI_OGNI_S = 60.0;
constexpr double CAMBIO_QUOTA_M = 5.0, POMPA_CAMBIO_M = 50.0, P_POMPA_CAMBIO = 0.7, P_POMPA_LANCIO = 0.5;
constexpr double POMPA_PATTUGLIA_MIN_S = 1800.0, POMPA_PATTUGLIA_MAX_S = 3600.0;
constexpr double P_TDU = 0.3;
constexpr double SCRICCHIOLIO_CAMBIO_M = 100.0;         // solo nei cambi di quota oltre 100 m
constexpr double SILENZIO = 0.25;
constexpr double POSTI_CONTATTO_M = 10000.0, POSTI_NORMALE_S = 900.0;   // i posti di combattimento (STIMA)

bool e_arma(const UnitaClassica& u) { return (u.flag & 0x20) != 0 || u.categoria == "arma"; }

double fattore_equipaggio(Esperienza e) { return e == Esperienza::recluta ? 3.0 : e == Esperienza::esperto ? 0.4 : 1.0; }

}  // namespace

void Missione::scafo_transitorio(const UnitaClassica& u, const char* voce) {
    if (!catalogo_ || !catalogo_->transitorio(voce)) return;
    if (prossimo_scafo_tr_ > SCAFO_A) prossimo_scafo_tr_ = SCAFO_DA;   // i vecchi sono finiti da un pezzo
    const int id = prossimo_scafo_tr_++;
    ++scafo_nati_[voce];
    transitori_armi_.push_back({id, voce, u.pos, std::max(u.quota, 1.0), tempo_, u.id,
                                seme_ ^ (static_cast<std::uint64_t>(id) * 0xD6E8FEB86659FD93ULL), 0.0});
}

void Missione::passo_transitori_scafo(double h) {
    if (!catalogo_) return;
    // gli scossoni nuovi del passo: gli urti (collisioni, incagli, ghiaccio) e le esplosioni dei colpi
    std::set<int> urtati;
    for (; urti_scafo_visti_ < urti_.size(); ++urti_scafo_visti_) {
        urtati.insert(urti_[urti_scafo_visti_].urto.a.unita);
        urtati.insert(urti_[urti_scafo_visti_].urto.b.unita);
    }
    std::vector<Vec2> scoppi;
    for (; colpi_scafo_visti_ < colpi_.size(); ++colpi_scafo_visti_) scoppi.push_back(colpi_[colpi_scafo_visti_].pos);
    // le emersioni d'emergenza dell'IA (registro, da DanniSottomarini): la soffiata delle casse
    const auto& ev = registro_.eventi();
    for (; eventi_scafo_visti_ < ev.size(); ++eventi_scafo_visti_)
        if (ev[eventi_scafo_visti_].tipo == TipoEvento::emersione_emergenza)
            if (const auto* u = trova(ev[eventi_scafo_visti_].unita)) scafo_transitorio(*u, "soffiata_emergenza");
    // i lanci nuovi: la pompa di compensazione del lanciatore
    std::set<int> lanciatori;
    for (const auto& [id, w] : unita_)
        if (e_arma(w) && !(w.flag & 0x80) && armi_scafo_viste_.insert(id).second) lanciatori.insert(w.lanciatore);
    const double ora_locale = std::fmod(std::fmod((ora_utc_s_ + tempo_) / 3600.0 + longitudine_ / 15.0, 24.0) + 24.0, 24.0);
    const int guardia = static_cast<int>(std::floor(ora_locale / 4.0));
    for (const auto& [id, u] : unita_) {
        if (u.categoria != "sottomarino" || u.distrutta || u.persa || e_arma(u)) continue;
        auto& st = scafi_tr_[id];
        if (!st.caso) {
            st.caso.emplace(seme_ ^ ((static_cast<std::uint64_t>(id) + 11) * 0x8CB92BA72F3D8DD7ULL));
            st.quota_prec = st.quota_partenza = st.quota_scricchiolio = u.quota;
            st.prossima_pompa = tempo_ + POMPA_PATTUGLIA_MIN_S + (POMPA_PATTUGLIA_MAX_S - POMPA_PATTUGLIA_MIN_S) * st.caso->uniforme();
            st.guardia = guardia;
        }
        Casuale& c = *st.caso;
        const double rateo = h > 0.0 ? (u.quota - st.quota_prec) / h : 0.0;
        const bool immerso = u.quota > IMMERSO_M;
        const double f = fattore_equipaggio(u.giocatore ? equipaggio_giocatore_ : esperienza_di_ia(u));
        const bool silenzio = u.giocatore ? (silenzio_giocatore_ || posti_) : allerta_unita(id).value_or(0) >= 2;
        const double fs = f * (silenzio ? SILENZIO : 1.0);
        const auto a_caso = [&](double per_ora) { return c.uniforme() < per_ora * fs * h / 3600.0; };
        if (immerso) {
            if (a_caso(PORTELLO_H)) scafo_transitorio(u, "portello_sbatte");
            bool scosso = urtati.count(id) > 0;
            for (const Vec2& p : scoppi) scosso = scosso || distanza(p, u.pos) <= SCOSSONE_M;
            if (a_caso(ATTREZZO_H) || (scosso && c.uniforme() < P_SCOSSONE)) scafo_transitorio(u, "attrezzo_cade");
            // i piani: un'inversione brusca del rateo di quota
            if (std::fabs(st.rateo_prec) > PIANI_INVERSIONE_M_S && rateo * st.rateo_prec < 0.0 && tempo_ - st.ultimo_piani >= PIANI_OGNI_S) {
                scafo_transitorio(u, "piani_colpo");
                st.ultimo_piani = tempo_;
            }
        }
        // i cambi di quota: lo scafo scricchiola scendendo e salendo oltre 100 m; a quota raggiunta la pompa d'assetto
        if (!st.in_cambio && std::fabs(u.quota_ordinata - u.quota) > CAMBIO_QUOTA_M) {
            st.in_cambio = true;
            st.quota_partenza = st.quota_scricchiolio = u.quota;
        }
        if (st.in_cambio) {
            const double collaudo = std::max(u.quota_max, 1.0);
            const double passo = u.quota > 0.7 * collaudo ? 25.0 : u.quota > 0.4 * collaudo ? 50.0 : 0.0;
            if (std::fabs(u.quota - st.quota_partenza) < SCRICCHIOLIO_CAMBIO_M || passo <= 0.0) {
                if (passo <= 0.0) st.quota_scricchiolio = u.quota;
            } else if (std::fabs(u.quota - st.quota_scricchiolio) >= passo) {
                scafo_transitorio(u, "scafo_scricchiola");
                st.quota_scricchiolio = u.quota;
            }
            if (std::fabs(u.quota_ordinata - u.quota) < 1.0) {
                st.in_cambio = false;
                if (std::fabs(u.quota - st.quota_partenza) > POMPA_CAMBIO_M && !silenzio && c.uniforme() < P_POMPA_CAMBIO)
                    st.pompa_alle = tempo_ + 30.0 + 90.0 * c.uniforme();
            }
        }
        if (lanciatori.count(id) && c.uniforme() < P_POMPA_LANCIO) st.pompa_alle = tempo_ + 10.0 + 30.0 * c.uniforme();
        if (st.pompa_alle >= 0.0 && tempo_ >= st.pompa_alle) {
            if (immerso) scafo_transitorio(u, "pompa_avvio");
            st.pompa_alle = -1.0;
        }
        if (tempo_ >= st.prossima_pompa) {   // in pattuglia
            if (immerso && !silenzio) scafo_transitorio(u, "pompa_avvio");
            st.prossima_pompa = tempo_ + POMPA_PATTUGLIA_MIN_S + (POMPA_PATTUGLIA_MAX_S - POMPA_PATTUGLIA_MIN_S) * c.uniforme();
        }
        // la TDU al cambio di guardia: mai in silenzio, ai posti o con un contatto ostile tenuto
        if (guardia != st.guardia) {
            st.guardia = guardia;
            bool ostile = false;
            for (const auto& [k, x] : u.contatti)
                if (const auto* b = trova(k); b && x.qualita >= 800.0 && ostili(u, *b)) ostile = true;
            if (immerso && !silenzio && !ostile && c.uniforme() < std::min(1.0, P_TDU * f)) scafo_transitorio(u, "tdu");
        }
        st.quota_prec = u.quota;
        st.rateo_prec = rateo;
    }
}

// chi ascolta i transitori dello scafo nuovi: l'IA coi suoi sensori passivi, il modello acustico, lo strato e il
// rumore proprio (come i lanci, missione_difesa.cpp); il giocatore li ha gia' nella scena del suo sonar
void Missione::ascolta_transitori_scafo() {
    if (!catalogo_) return;
    for (const Transitorio& t : transitori_armi_) {
        if (t.id < SCAFO_DA || t.id > SCAFO_A || tempo_ < t.t0 + 1.0) continue;
        const auto* o = trova(t.origine);
        if (!o) continue;
        // la prima volta (un secondo dopo l'inizio): il contatto e l'allerta; poi, finche' suona, al sonarista ogni passo
        const bool primo = scafo_tr_sentiti_.insert(t.id).second;
        const auto s = sorgente_transitoria(*catalogo_, t, primo ? t.t0 + 1.0 : tempo_, 1.0);
        if (!s) continue;
        const bool soffiata = t.tipo == "soffiata_emergenza";
        for (auto& [id, v] : unita_) {
            if (id == t.origine || v.giocatore || v.distrutta || v.persa || e_arma(v) || (v.categoria != "sottomarino" && v.categoria != "nave")) continue;
            if (!primo && sonaristi_ia_spenti_) continue;
            std::vector<Ricevitore> ric;
            for (const Sensore* sn : sensori_acustici(v, TipoSensori::passivi)) ric.push_back(ricevitore_acustico(v, sn));
            if (ric.empty()) continue;
            const AscoltoTransitorio h = ascolta_transitorio(*s, ric, ambiente_acustico(v.pos, t.pos));
            if (!h.sentito) continue;
            // al sonarista dell'IA, anche se il passivo non lo sente (lo crea, se non c'e'): nel suo secondo diventa
            // POSSUB o PROBSUB (nucleo/sonarista, transitorio_scafo)
            if (!sonaristi_ia_spenti_) {
                ascolto_ia(v, o->id, rilevamento(v.pos, o->pos), false, nullptr, h.se_db, h.errore_deg);
                AscoltoSonar& a = sonaristi_ia_.at(id).ascolti[o->id];
                a.sottomarino = true;   // la verita': un transitorio dello scafo viene da un sottomarino
                a.transitorio_scafo = true;
            }
            if (!primo) continue;
            scafo_sentiti_.push_back({tempo_, id, o->id, t.tipo, h.se_db});
            const bool c_era = v.contatti.count(o->id) > 0;
            if (Contatto* c = c_era ? &v.contatti.at(o->id) : nuovo_contatto(v, o->id)) {
                c->qualita = std::max(c->qualita, 400.0);
                c->lampeggio = 0;
                c->ultima_scoperta = tempo_;
                c->rilevamento = normalizza_360(h.rilevamento + h.errore_deg * gauss());
                c->indizio_transitorio = true;
                if (c->sensore.empty()) c->sensore = "transitorio";
            }
            // un transitorio di scafo dice «sottomarino»: l'allerta come un POSSUB, o un PROBSUB sul contatto che c'era
            // (portello, TDU, attrezzo e piani sono l'indisciplina: niente evasione); la soffiata si sente da lontano
            if (forze_ && ostili(v, *o)) evento_allerta(id, c_era || soffiata ? "probsub" : "possub");
        }
    }
    std::erase_if(scafo_tr_sentiti_, [&](int id) {
        return std::none_of(transitori_armi_.begin(), transitori_armi_.end(), [&](const Transitorio& t) { return t.id == id; });
    });
}

void Missione::passo_posti() {
    const UnitaClassica* g = giocatore();
    if (!g || g->distrutta || g->persa) return;
    std::string causa;
    for (; posti_allarmi_visti_ < conti_.allarmi_siluro.size(); ++posti_allarmi_visti_) causa = "siluro";
    if (causa.empty())
        for (const auto& [id, w] : unita_)
            if (e_arma(w) && !(w.flag & 0x80) && w.lanciatore == g->id && !armi_scafo_viste_.count(id)) causa = "lancio";
    if (causa.empty())
        for (const auto& [k, c] : g->contatti)
            if (const auto* b = trova(k); b && !e_arma(*b) && c.durata >= 800.0 && ostili(*g, *b) && distanza(g->pos, b->pos) <= POSTI_CONTATTO_M) {
                causa = "contatto";
                break;
            }
    if (!causa.empty()) {
        posti_ultimo_ = tempo_;
        if (!posti_) {
            posti_ = true;
            cambi_posti_.push_back({tempo_, true, causa});
        }
    } else if (posti_ && tempo_ - posti_ultimo_ > POSTI_NORMALE_S) {
        posti_ = false;
        cambi_posti_.push_back({tempo_, false, "normale"});
    }
}

}  // namespace m688
