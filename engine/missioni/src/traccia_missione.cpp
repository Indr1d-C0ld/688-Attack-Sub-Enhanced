#include "missioni/traccia_missione.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <map>
#include <set>

#include "missioni/griglia_ghiaccio.hpp"
#include "missioni/registro_missione.hpp"

namespace m688 {

namespace {

constexpr double MINA_AGGIRATA_M = 200.0;    // come gli achievement (achievement_missione.cpp)
constexpr int MSG_CHIGLIA = 487;             // «chiglia a prua» (missione_ghiaccio.cpp)

const char* nome_evento_siluro(EventoSiluro::Tipo t) {
    switch (t) {
        case EventoSiluro::Tipo::attivazione: return "attivazione";
        case EventoSiluro::Tipo::aggancio: return "aggancio";
        case EventoSiluro::Tipo::perdita: return "perdita";
        case EventoSiluro::Tipo::esca_sedotta: return "esca_sedotta";
        case EventoSiluro::Tipo::esca_rifiutata: return "esca_rifiutata";
        case EventoSiluro::Tipo::filo_rotto: return "filo_rotto";
        case EventoSiluro::Tipo::spento: return "spento";
        case EventoSiluro::Tipo::esaurito: return "esaurito";
        case EventoSiluro::Tipo::esplosione: return "esplosione";
        case EventoSiluro::Tipo::ping: return "ping";
    }
    return "?";
}

}  // namespace

std::vector<EventoIa> RaccoltaTraccia::nuovi(const Missione& m) {
    std::vector<EventoIa> out;
    if (!m.con_acustica()) return out;
    const double t = m.tempo();
    const auto trova = [&](int id) -> const UnitaClassica* {
        const auto it = m.unita().find(id);
        return it == m.unita().end() ? nullptr : &it->second;
    };
    if (m.con_statistiche()) {   // R1-R6 del runtime: i fatti al passo esatto
        for (; r1_ < m.scoperte_contatti().size(); ++r1_) {
            const auto& x = m.scoperte_contatti()[r1_];
            EventoIa e;
            e.t = x.t; e.unita = x.osservatore; e.altra = x.bersaglio; e.a = x.distanza_m; e.b = x.eccesso_db; e.s = x.sensore;
            using T = Missione::ScopertaContatto::Tipo;
            e.tipo = x.tipo == T::nuovo ? "contatto_nuovo" : x.tipo == T::solido ? "contatto_solido" : "contatto_perso";
            if (const UnitaClassica* o = trova(x.osservatore); !o || o->categoria != "arma") out.push_back(std::move(e));
        }
        for (; r2_ < m.scoperte_possibili().size(); ++r2_) {
            const auto& x = m.scoperte_possibili()[r2_];
            EventoIa e;
            e.t = x.t; e.tipo = "possibile"; e.unita = x.osservatore; e.altra = x.bersaglio; e.a = x.distanza_m;
            e.b = x.eccesso_db; e.s = x.sensore;
            out.push_back(std::move(e));
        }
        for (; r3_ < m.lanci().size(); ++r3_) {
            const auto& x = m.lanci()[r3_];
            EventoIa e;
            e.t = x.t; e.tipo = "lancio"; e.unita = x.lanciatore; e.altra = x.bersaglio; e.n = x.arma; e.s = x.classe_arma;
            if (x.stima) {
                e.m = 1;
                e.a = x.stima->pos.x; e.b = x.stima->pos.y;
                e.c = x.stima->rotta.value_or(-1.0); e.d = x.stima->velocita_kn.value_or(-1.0);
            }
            out.push_back(std::move(e));
        }
        for (; tma_ < m.eventi_tma_ia().size(); ++tma_) {
            const auto& x = m.eventi_tma_ia()[tma_];
            if (x.tipo != Missione::EventoTmaIa::Tipo::soluzione || !x.stima) continue;
            EventoIa e;
            e.t = x.t; e.tipo = "tma_soluzione"; e.unita = x.unita; e.altra = x.bersaglio; e.n = x.gambe;
            e.m = x.stima->osservabile;
            e.a = x.stima->posizione.x; e.b = x.stima->posizione.y;
            e.c = x.stima->soluzione.rotta; e.d = x.stima->soluzione.velocita_m_s / 0.514444;
            out.push_back(std::move(e));
        }
        for (; r4_ < m.evasioni().size(); ++r4_) {
            const auto& x = m.evasioni()[r4_];
            EventoIa e;
            e.t = x.t; e.tipo = "evasione"; e.unita = x.unita; e.altra = x.minaccia; e.s = x.causa;
            out.push_back(std::move(e));
        }
        for (; r5_ < m.emissioni().size(); ++r5_) {
            const auto& x = m.emissioni()[r5_];
            EventoIa e;
            e.t = x.t; e.tipo = "emissione"; e.unita = x.unita; e.a = x.durata_s; e.s = x.tipo;
            out.push_back(std::move(e));
            if (x.tipo == "radar" && x.durata_s < 0.0) radar_aperti_.push_back(r5_);
        }
        for (auto it = radar_aperti_.begin(); it != radar_aperti_.end();) {   // il radar spento: la voce si chiude
            const auto& x = m.emissioni()[*it];
            if (x.durata_s < 0.0) { ++it; continue; }
            EventoIa e;
            e.t = x.t + x.durata_s; e.tipo = "emissione_fine"; e.unita = x.unita; e.a = x.durata_s; e.s = x.tipo;
            out.push_back(std::move(e));
            it = radar_aperti_.erase(it);
        }
        for (; r6_ < m.punti_nave_ia().size(); ++r6_) {
            const auto& x = m.punti_nave_ia()[r6_];
            EventoIa e;
            e.t = x.t; e.tipo = "punto_nave"; e.unita = x.unita; e.m = x.fatto; e.a = x.spostamento_m;
            e.b = x.sigma_prima_m; e.c = x.sigma_dopo_m; e.s = x.tipo;
            out.push_back(std::move(e));
        }
    } else {   // senza R1: i contatti di tutte le unita' dai cambi delle loro liste
        std::set<std::pair<int, int>> presenti;
        for (const auto& [id, u] : m.unita()) {
            if (u.distrutta || u.persa || u.categoria == "arma") continue;
            for (const auto& [k, c] : u.contatti) {
                const std::pair<int, int> chiave{id, k};
                presenti.insert(chiave);
                const bool solido = c.qualita >= SOLIDO;
                const auto riga = [&](const char* tipo) {
                    EventoIa e;
                    e.t = t; e.tipo = tipo; e.unita = id; e.altra = k; e.s = c.sensore; e.b = c.eccesso_db;
                    if (const UnitaClassica* b = trova(k)) e.a = distanza(u.pos, b->pos);
                    out.push_back(std::move(e));
                };
                auto it = contatti_.find(chiave);
                if (it == contatti_.end()) {
                    riga("contatto_nuovo");
                    if (solido) riga("contatto_solido");
                    contatti_[chiave] = {solido, -1.0};
                    continue;
                }
                it->second.sparito = -1.0;   // tornato prima del perso
                if (solido && !it->second.solido) riga("contatto_solido");
                it->second.solido = solido;
            }
        }
        for (auto it = contatti_.begin(); it != contatti_.end();) {
            if (presenti.count(it->first)) { ++it; continue; }
            if (it->second.sparito < 0.0) it->second.sparito = t;
            if (t - it->second.sparito < ASSENZA_PERSO_S) { ++it; continue; }
            EventoIa e;
            e.t = it->second.sparito; e.tipo = "contatto_perso"; e.unita = it->first.first; e.altra = it->first.second;
            const UnitaClassica* o = trova(it->first.first);
            const UnitaClassica* b = trova(it->first.second);
            if (o && b) e.a = distanza(o->pos, b->pos);
            out.push_back(std::move(e));
            it = contatti_.erase(it);
        }
    }
    // gli eventi dei siluri del modello
    for (; siluri_visti_ < m.eventi_siluri().size(); ++siluri_visti_) {
        const EventoSiluroMissione& x = m.eventi_siluri()[siluri_visti_];
        EventoIa e;
        e.t = x.evento.tempo; e.tipo = "siluro"; e.unita = x.siluro; e.altra = x.lanciatore; e.n = x.evento.sorgente;
        e.a = x.evento.pos.x; e.b = x.evento.pos.y; e.c = x.evento.quota; e.s = nome_evento_siluro(x.evento.tipo);
        out.push_back(std::move(e));
    }
    return out;
}

std::vector<ContattoUnita> RaccoltaTraccia::contatti(const Missione& m) {
    std::vector<ContattoUnita> out;
    if (!m.con_acustica()) return out;
    const double t = m.tempo();
    // la classe che ciascuno ha dato (i sonaristi dell'IA e il giocatore), dal registro
    const auto& reg = m.eventi_missione();
    const int giocatore = m.giocatore() ? m.giocatore()->id : -1;
    for (; registro_ < reg.size(); ++registro_) {
        const EventoMissione& e = reg[registro_];
        if (e.tipo == TipoEvento::classificazione_ia && !e.classe_creduta.empty()) credute_[{e.unita, e.altra}] = e.classe_creduta;
        if (e.tipo == TipoEvento::classificazione && !e.classe_creduta.empty() && giocatore >= 0)
            credute_[{giocatore, e.unita}] = e.classe_creduta;
    }
    std::set<std::pair<int, int>> presenti;
    for (const auto& [id, u] : m.unita()) {
        if (u.distrutta || u.persa || u.categoria == "arma") continue;
        for (const auto& [k, c] : u.contatti) {
            const std::pair<int, int> chiave{id, k};
            presenti.insert(chiave);
            assenti_.erase(chiave);
            ContattoUnita r;
            r.t = t; r.osservatore = id; r.bersaglio = k;
            r.qualita = c.qualita; r.rilevamento = c.rilevamento; r.eta_s = std::max(0.0, t - c.ultima_scoperta);
            r.solido = c.qualita >= SOLIDO; r.posizione_nota = c.posizione_nota;
            r.pos_stimata = c.pos_stimata; r.sensore = c.sensore;
            if (const auto it = credute_.find(chiave); it != credute_.end()) r.classe_creduta = it->second;
            const auto it = scritte_.find(chiave);
            bool scrivi = it == scritte_.end();
            if (!scrivi) {
                const ContattoUnita& p = it->second;
                const bool stato = p.solido != r.solido || p.posizione_nota != r.posizione_nota || p.sensore != r.sensore ||
                                   p.classe_creduta != r.classe_creduta;
                const bool varia = distanza(p.pos_stimata, r.pos_stimata) >= SPOSTA_K_M ||
                                   std::fabs(normalizza_180(p.rilevamento - r.rilevamento)) >= RUOTA_K_GRADI ||
                                   std::fabs(p.qualita - r.qualita) >= QUALITA_K;
                scrivi = stato || (varia && t - p.t >= RINNOVO_K_S - 1e-6);
            }
            if (!scrivi) continue;
            scritte_[chiave] = r;
            out.push_back(std::move(r));
        }
    }
    for (auto it = scritte_.begin(); it != scritte_.end();) {   // i persi, dopo l'assenza
        if (presenti.count(it->first)) { ++it; continue; }
        const double da = assenti_.try_emplace(it->first, t).first->second;
        if (t - da < ASSENZA_PERSO_S) { ++it; continue; }
        ContattoUnita r = it->second;
        r.t = da;
        r.perso = true;
        out.push_back(std::move(r));
        assenti_.erase(it->first);
        it = scritte_.erase(it);
    }
    return out;
}

std::vector<QuadroParte> RaccoltaTraccia::quadri(const Missione& m) {
    std::vector<QuadroParte> out;
    if (!m.con_acustica() || m.tempo() < prossimo_p_ - 1e-9) return out;
    prossimo_p_ = m.tempo() + PASSO_P_S;
    for (const int lato : {0x08, 0x10, 0}) {
        QuadroParte q;
        q.t = m.tempo();
        q.lato = lato;
        for (const Missione::VoceQuadro& v : m.quadro_parte(lato)) {
            VoceParte x;
            x.bersaglio = v.bersaglio; x.posizione = v.posizione; x.pos = v.pos; x.origine = v.origine;
            x.rilevamento = v.rilevamento; x.semiasse_maggiore_m = v.semiasse_maggiore_m;
            x.semiasse_minore_m = v.semiasse_minore_m; x.orientamento_deg = v.orientamento_deg; x.t_dato = v.t_dato;
            x.sensore = v.sensore; x.fonte = v.fonte; x.categoria = v.categoria; x.classe = v.classe; x.qualita = v.qualita;
            q.voci.push_back(std::move(x));
        }
        const auto it = quadri_scritti_.find(lato);
        bool cambia = it == quadri_scritti_.end() ? !q.voci.empty() : it->second.voci.size() != q.voci.size();
        if (!cambia && it != quadri_scritti_.end()) {
            std::map<int, const VoceParte*> prima;
            for (const VoceParte& v : it->second.voci) prima[v.bersaglio] = &v;
            for (const VoceParte& v : q.voci) {
                const auto p = prima.find(v.bersaglio);
                if (p == prima.end()) { cambia = true; break; }
                const VoceParte& a = *p->second;
                if (a.posizione != v.posizione || a.classe != v.classe || a.categoria != v.categoria || a.fonte != v.fonte ||
                    a.sensore != v.sensore || distanza(a.pos, v.pos) >= SPOSTA_P_M ||
                    std::fabs(normalizza_180(a.rilevamento - v.rilevamento)) >= 1.0) {
                    cambia = true;
                    break;
                }
            }
        }
        if (!cambia) continue;
        quadri_scritti_[lato] = q;
        out.push_back(std::move(q));
    }
    return out;
}

Fotogramma fotogramma_da(const Missione& m, const StazioniMissione* st, const DatiApp& app, int dopo_evento,
                         RaccoltaTraccia* raccolta) {
    Fotogramma f;
    if (raccolta) {
        f.eventi_ia = raccolta->nuovi(m);
        f.contatti_unita = raccolta->contatti(m);
        f.quadri_parte = raccolta->quadri(m);
        f.interventi = raccolta->interventi(m);   // il vetrino di Dio (record X)
    }
    f.t = m.tempo();
    f.app = app;
    const auto& eventi = m.eventi_missione();
    if (!eventi.empty()) f.evento = eventi.back().id;
    for (auto it = eventi.rbegin(); it != eventi.rend() && it->id > dopo_evento; ++it) {   // i nuovi, dalla coda
        EventoTraccia e;
        e.id = it->id;
        e.tempo = it->tempo;
        e.tipo = nome_evento(it->tipo);
        e.testo_it = it->testo_it;
        e.testo_en = it->testo_en;
        e.interesse = it->interesse;
        e.noto = it->noto;
        e.unita = it->unita;
        e.altra = it->altra;
        e.distanza_m = it->distanza_m;
        e.classe = it->classe;
        e.categoria = it->categoria;
        e.classe_arma = it->classe_arma;
        e.lato = static_cast<int>(it->lato);
        e.giusta = it->giusta;
        e.livello = it->livello;
        e.tempo_scoperta = it->tempo_scoperta;
        e.codice = it->codice;
        e.valore = it->valore;
        e.arma = it->arma;
        e.classe_creduta = it->classe_creduta;
        e.pos = it->pos;
        f.eventi.push_back(std::move(e));
    }
    std::reverse(f.eventi.begin(), f.eventi.end());
    f.unita.reserve(m.unita().size());
    for (const auto& [id, u] : m.unita()) {
        StatoUnita s;
        s.id = id;
        s.classe = u.classe;
        s.categoria = u.categoria;
        s.flag = u.flag;
        s.pos = u.pos;
        s.quota = u.quota;
        s.rotta = u.rotta;
        s.velocita_kn = u.velocita;
        s.distrutta = u.distrutta;
        s.persa = u.persa;
        s.incagliata = u.incagliata;
        s.sul_fondo = u.sul_fondo;
        s.alberi = u.flag54 & 0x07;
        s.lanciatore = u.lanciatore;
        s.collegata = u.collegata;
        s.pos_stimata = m.posizione_stimata(id);
        if (const auto el = m.ellisse_navigazione(id)) {
            s.sigma_nav_m = el->maggiore_m;
            s.sigma_min_m = el->minore_m;
            s.orientamento_deg = el->orientamento_deg;
        }
        s.allerta = m.allerta_unita(id).value_or(-1);
        if (const auto tm = m.tma_ia(id)) {   // la TMA in corso dell'IA (lettura «unita'» della sandbox)
            s.tma_bersaglio = tm->bersaglio;
            s.tma_gambe = tm->gambe;
            if (tm->soluzione) {
                s.tma_soluzione = true;
                s.tma_osservabile = tm->soluzione->osservabile;
                s.tma_pos = tm->soluzione->posizione;
                s.tma_rotta = tm->soluzione->soluzione.rotta;
                s.tma_velocita_kn = tm->soluzione->soluzione.velocita_m_s / 0.514444;
            }
        }
        s.armi_rimaste = m.armi_rimaste(id).value_or(-1);
        s.tubi_carichi = m.tubi_carichi(id).value_or(-1);
        f.unita.push_back(std::move(s));
    }
    if (const UnitaClassica* g = m.giocatore(); g && m.radar_giocatore_acceso())   // le altre emissioni: R5 del runtime
        for (StatoUnita& s : f.unita)
            if (s.id == g->id) s.emissioni |= StatoUnita::EMESSO_RADAR;
    // il quadro: i contatti che il giocatore vede, con l'ultima classe che ha dato (lui o il suo sonarista)
    if (const UnitaClassica* g = m.giocatore()) {
        std::map<int, std::string> creduta;
        for (const EventoMissione& e : eventi)
            if (e.tipo == TipoEvento::classificazione && !e.classe_creduta.empty()) creduta[e.unita] = e.classe_creduta;
        for (const auto& [id, c] : g->contatti) {
            if (!c.visibile()) continue;
            StatoContatto s;
            s.id = id;
            s.lettera = c.lettera;
            s.qualita = c.qualita;
            s.durata = c.durata;
            s.rilevamento = c.rilevamento;
            s.specchio = c.rilevamento_specchio;
            s.posizione_nota = c.posizione_nota;
            s.pos_stimata = c.pos_stimata;
            s.quota_stimata = c.quota_stimata;
            s.sensore = c.sensore;
            if (const auto it = creduta.find(id); it != creduta.end()) s.classe_creduta = it->second;
            f.contatti.push_back(std::move(s));
        }
    }
    if (st) {
        f.rilevamenti = st->tma().rilevamenti();
        f.bersaglio_tma = st->bersaglio_tma();
    }
    return f;
}

std::vector<RecordIntervento> RaccoltaTraccia::interventi(const Missione& m) {
    std::vector<RecordIntervento> v;
    for (std::size_t k = 0; k < m.interventi().size() && k < m.stati_interventi().size(); ++k) {
        const Intervento& x = m.interventi()[k];
        const StatoIntervento& st = m.stati_interventi()[k];
        // come il registro: applicato, poi finito (non per gli istantanei), o soltanto fallito
        const bool istantaneo = st.finito && st.fine <= st.inizio;
        const int fase = st.fallito ? RecordIntervento::FASE_FALLITO : st.finito && !istantaneo ? RecordIntervento::FASE_FINITO
                         : st.applicato ? RecordIntervento::FASE_APPLICATO : 0;
        int& scritta = fasi_interventi_[st.id];
        while (scritta < fase) {
            scritta = st.fallito ? RecordIntervento::FASE_FALLITO : scritta + 1;
            RecordIntervento r;
            r.id = x.id;
            r.fase = scritta;
            r.t = scritta == RecordIntervento::FASE_FINITO ? st.fine : st.inizio;
            r.tipo = x.tipo;
            r.unita = x.unita;
            r.ha_punto = x.punto.has_value();
            if (x.punto) r.punto = *x.punto;
            r.raggio_m = x.raggio_m;
            r.durata_s = x.durata_s;
            std::map<std::string, std::string> chiavi = x.testi;
            for (const auto& [n, val] : x.parametri) {
                char b[40];
                std::snprintf(b, sizeof b, "%.17g", val);
                chiavi["#" + n] = b;   // i numeri col cancelletto, i testi senza
            }
            if (x.innesco) {   // l'innesco, coi punti esclamativi
                chiavi["!evento"] = x.innesco->evento;
                chiavi["!unita"] = std::to_string(x.innesco->unita);
                chiavi["!altra"] = std::to_string(x.innesco->altra);
                if (x.innesco->punto) {
                    char b[64];
                    std::snprintf(b, sizeof b, "%.17g;%.17g;%.17g", x.innesco->punto->x, x.innesco->punto->y, x.innesco->raggio_m);
                    chiavi["!zona"] = b;
                }
            }
            r.chiavi = codifica_chiavi(chiavi);
            r.riga_it = x.riga_it;
            r.riga_en = x.riga_en;
            v.push_back(std::move(r));
        }
    }
    return v;
}

void metadati_conti(const Missione& m, RegistratoreTraccia& r) {
    // il mese dell'acqua per il profilo del suono nella scheda dello spettatore; la griglia del ghiaccio la scrive main, prima del primo passo
    if (m.sandbox() && m.profilo_in({}, {})) r.metadato(METADATO_MESE_ACUSTICA, std::to_string(m.mese_acustica()));
    const UnitaClassica* g = m.giocatore();
    if (!g) return;
    const auto& k = m.conti_giocatore();
    const auto metti = [&](const char* nome, double v) {
        char b[48];
        std::snprintf(b, sizeof b, "%.6g", v);
        r.metadato(std::string("conti.") + nome, b);
    };
    metti("sotto_banchisa_s", k.sotto_banchisa_s);
    metti("emersioni_ghiaccio", k.emersioni_ghiaccio);
    metti("visive_fanali_giuste", k.visive_fanali_giuste);
    metti("visive_luci_spente", k.visive_luci_spente);
    metti("visive_notte_sagoma", k.visive_notte_sagoma);
    metti("errate_luci_spente", k.errate_luci_spente);
    metti("in_coda_s", k.in_coda_s);
    metti("mine_aggirate", static_cast<double>(std::count_if(k.mine_vicine_m.begin(), k.mine_vicine_m.end(),
                                                             [](const auto& x) { return x.second >= MINA_AGGIRATA_M; })));
    metti("salvati", m.salvati().salvati);
    int urti = 0;
    for (const auto& u : m.urti()) urti += u.urto.a.unita == g->id && u.urto.b.unita < 0;
    metti("urti_ghiaccio", urti);
    int chiglie = 0;
    for (const Messaggio& x : m.messaggi()) chiglie += x.unita == g->id && x.codice == MSG_CHIGLIA;
    metti("avvisi_chiglia", chiglie);
    // il tempo con le luci proprie accese (dai cambi; prima del primo, spente)
    double accese = 0.0, da = -1.0;
    for (const auto& c : m.cambi_luci()) {
        if (c.unita != g->id) continue;
        if (c.stato != luci::Stato::spente && da < 0.0) da = c.tempo;
        else if (c.stato == luci::Stato::spente && da >= 0.0) accese += c.tempo - da, da = -1.0;
    }
    if (da >= 0.0) accese += m.tempo() - da;
    metti("luci_accese_s", accese);
    // le volte che un sottomarino ci ha classificati (il pedinato che si accorge di noi)
    int classificati = 0;
    for (const auto& c : m.classificazioni_ia()) {
        const auto it = m.unita().find(c.unita);
        classificati += c.sorgente == g->id && it != m.unita().end() && it->second.categoria == "sottomarino";
    }
    metti("classificati_da_sottomarini", classificati);
    // il sonar delle mine del battello, per le mine scoperte per sensore (missione_bqs15.cpp: il BQS-15 dei 688, il BQS-8
    // degli Sturgeon, l'MG-24 dell'Alfa, l'MG-70 degli altri sovietici)
    r.metadato("conti.sensore_mine", g->classe == "los_angeles" ? "bqs15" : g->classe == "sturgeon" ? "bqs8" : g->classe == "alfa" ? "mg24"
                                     : (g->flag & 0x10) ? "mg70" : "bqs15");
}

}  // namespace m688
