// Sandbox V2 (docs/19_sandbox.md §3 e §4; nota condivisa sandbox-v2-runtime-contratto.md): le liste dei fatti di tutte
// le unita' per le statistiche tecniche (R1-R6) e il quadro di una parte. Solo Enhanced e solo con
// registra_statistiche: la partita normale non registra niente.

#include <algorithm>
#include <cmath>
#include <map>
#include <set>

#include "missioni/missione.hpp"
#include "nucleo/geometria.hpp"

namespace m688 {

namespace {

const char* nome_canale(Missione::MessaggioIa::Canale c) {
    switch (c) {
        case Missione::MessaggioIa::Canale::formazione: return "formazione";
        case Missione::MessaggioIa::Canale::aereo_nave: return "aereo_nave";
        case Missione::MessaggioIa::Canale::teatro: return "teatro";
        case Missione::MessaggioIa::Canale::broadcast: return "broadcast";
        case Missione::MessaggioIa::Canale::giocatore: return "giocatore";
    }
    return "";
}

}  // namespace

// R2: il primo istante in cui la fisica consente la scoperta, una volta per terna
void Missione::possibile(int osservatore, int bersaglio, const std::string& sensore, double eccesso_db) {
    if (!statistiche_ || eccesso_db < 0.0 || osservatore == bersaglio) return;
    if (!possibili_visti_.insert({osservatore, bersaglio, sensore}).second) return;
    const auto o = unita_.find(osservatore), b = unita_.find(bersaglio);
    const double d = o != unita_.end() && b != unita_.end() ? distanza(o->second.pos, b->second.pos) : 0.0;
    scoperte_possibili_.push_back({tempo_, osservatore, bersaglio, sensore, d, eccesso_db});
}

// R3: il lancio con quello che il lanciatore sapeva del bersaglio
void Missione::lancio_per_statistiche(int arma, const UnitaClassica* l, int bersaglio, const std::string& classe) {
    if (!statistiche_) return;
    LancioArma x{tempo_, l ? l->id : -1, arma, bersaglio, classe, std::nullopt};
    const auto da_tma = [](const StimaTma& s, const char* da) {
        StimaLancio k;
        k.pos = s.posizione;
        k.rotta = s.soluzione.rotta;
        k.velocita_kn = s.soluzione.velocita_m_s / NODO;
        k.sigma_distanza_m = s.sigma_distanza_m;
        k.sigma_trasversale_m = s.sigma_trasversale_m;
        k.da = da;
        return k;
    };
    if (l && bersaglio >= 0) {
        const auto k = l->contatti.find(bersaglio);
        if (l->giocatore && soluzione_giocatore_ && soluzione_giocatore_->first == bersaglio) {
            x.stima = da_tma(soluzione_giocatore_->second, "giocatore");
        } else if (const auto t = tma_ia_.find({l->id, bersaglio}); t != tma_ia_.end() && t->second.stima && t->second.stima->osservabile) {
            x.stima = da_tma(*t->second.stima, "tma");
        } else if (k != l->contatti.end() && k->second.posizione_nota) {
            StimaLancio s;
            s.pos = k->second.pos_stimata;
            s.da = "contatto";
            x.stima = s;
        }
    }
    if (l && l->giocatore) soluzione_giocatore_.reset();
    lanci_.push_back(std::move(x));
}

// R5: i radar accesi in questo passo (unita', radar); le voci si aprono all'accensione e si chiudono allo spegnimento
void Missione::radar_per_statistiche(const std::vector<std::pair<int, std::string>>& accesi) {
    if (!statistiche_) return;
    for (const auto& k : accesi)
        if (!radar_accesi_.count(k)) {
            radar_accesi_[k] = emissioni_.size();
            emissioni_.push_back({tempo_, k.first, "radar", -1.0, k.second});
        }
    for (auto it = radar_accesi_.begin(); it != radar_accesi_.end();) {
        if (std::find(accesi.begin(), accesi.end(), it->first) != accesi.end()) { ++it; continue; }
        auto& e = emissioni_[it->second];
        e.durata_s = tempo_ - e.t;
        it = radar_accesi_.erase(it);
    }
}

// R1 e la radio di R5, a fine passo
void Missione::passo_statistiche() {
    std::set<std::pair<int, int>> visti;
    for (const auto& [id, u] : unita_) {
        if (u.distrutta || u.persa) continue;
        for (const auto& [b, c] : u.contatti) {
            const std::pair<int, int> k{id, b};
            visti.insert(k);
            const auto bu = unita_.find(b);
            const double d = bu != unita_.end() ? distanza(u.pos, bu->second.pos) : 0.0;
            auto it = stato_contatti_.find(k);
            if (it == stato_contatti_.end()) {
                scoperte_contatti_.push_back({tempo_, id, b, ScopertaContatto::Tipo::nuovo, c.sensore, d, c.eccesso_db});
                it = stato_contatti_.emplace(k, std::make_pair(false, c.sensore)).first;
            }
            if (!it->second.first && c.qualita >= 800.0) {
                scoperte_contatti_.push_back({tempo_, id, b, ScopertaContatto::Tipo::solido, c.sensore, d, c.eccesso_db});
                it->second.first = true;
            }
            if (!c.sensore.empty()) it->second.second = c.sensore;
        }
    }
    for (auto it = stato_contatti_.begin(); it != stato_contatti_.end();) {
        if (visti.count(it->first)) { ++it; continue; }
        const auto o = unita_.find(it->first.first);
        if (o != unita_.end() && !o->second.distrutta && !o->second.persa) {   // un osservatore perso non perde i contatti
            const auto b = unita_.find(it->first.second);
            const double d = b != unita_.end() ? distanza(o->second.pos, b->second.pos) : 0.0;
            scoperte_contatti_.push_back({tempo_, it->first.first, it->first.second, ScopertaContatto::Tipo::perso, it->second.second, d, 0.0});
        }
        it = stato_contatti_.erase(it);
    }
    // la radio: i messaggi dell'IA alla partenza, le trasmissioni del giocatore dal registro
    const auto& mi = messaggi_ia();
    for (; messaggi_ia_emessi_ < mi.size(); ++messaggi_ia_emessi_) {
        const auto& x = mi[messaggi_ia_emessi_];
        emissioni_.push_back({x.t_partenza, x.da, "radio", 0.0, nome_canale(x.canale)});
    }
    const auto& ev = registro_.eventi();
    for (; eventi_radio_visti_ < ev.size(); ++eventi_radio_visti_)
        if (ev[eventi_radio_visti_].tipo == TipoEvento::trasmissione)
            emissioni_.push_back({ev[eventi_radio_visti_].tempo, ev[eventi_radio_visti_].giocatore, "radio", 0.0, "giocatore"});
}

// --- il quadro di una parte ---

std::vector<Missione::VoceQuadro> Missione::quadro_parte(int lato) const {
    const auto parte = [](const UnitaClassica& u) { return u.flag & 0x18; };   // 24: la parte del luogo (M7)
    // la classe detta dai sonaristi dell'IA della parte, l'ultima per sorgente
    std::map<int, std::string> classi;
    for (const auto& k : classificazioni_ia_)
        if (const auto u = unita_.find(k.unita); u != unita_.end() && parte(u->second) == lato && !k.classe.empty()) classi[k.sorgente] = k.classe;
    struct Candidato { int rango; VoceQuadro v; };
    std::map<int, Candidato> meglio;
    std::map<int, std::set<int>> fonti;
    const auto proponi = [&](int rango, VoceQuadro v) {
        fonti[v.bersaglio].insert(v.fonte);
        auto it = meglio.find(v.bersaglio);
        if (it == meglio.end()) { meglio.emplace(v.bersaglio, Candidato{rango, std::move(v)}); return; }
        auto& c = it->second;
        if (rango < c.rango || (rango == c.rango && v.t_dato > c.v.t_dato)) c = Candidato{rango, std::move(v)};
    };
    for (const auto& [id, u] : unita_) {
        if (u.distrutta || u.persa || parte(u) != lato || (u.flag & 0x20)) continue;
        for (const auto& [b, c] : u.contatti) {
            const auto bu = unita_.find(b);
            if (bu == unita_.end() || bu->second.distrutta || bu->second.persa || (bu->second.flag & 0x20) || parte(bu->second) == lato) continue;
            VoceQuadro v;
            v.bersaglio = b;
            v.fonte = id;
            v.t_dato = c.ultima_scoperta;
            v.sensore = c.sensore;
            v.qualita = c.qualita;
            v.origine = u.pos;
            v.rilevamento = c.rilevamento;
            const auto k = classi.find(b);
            v.classe = k != classi.end() ? k->second : "";
            v.categoria = v.classe.empty() ? "sconosciuto" : bu->second.categoria;
            if (c.posizione_nota) {
                v.posizione = true;
                v.pos = c.pos_stimata;
                proponi(1, v);
            } else if (const auto s = stima_tma_ia(id, b); s && s->osservabile) {
                v.posizione = true;
                v.pos = s->posizione;
                v.sensore = "tma";
                // l'ellisse: lungo il rilevamento l'incertezza della distanza, di traverso quella del rilevamento
                const double lungo = s->sigma_distanza_m, traverso = s->sigma_trasversale_m;
                v.semiasse_maggiore_m = std::max(lungo, traverso);
                v.semiasse_minore_m = std::min(lungo, traverso);
                v.orientamento_deg = normalizza_360(c.rilevamento + (lungo >= traverso ? 0.0 : 90.0));
                proponi(2, v);
            } else {
                proponi(4, v);
            }
        }
        for (const auto& [b, r] : quadro_radio(id)) {
            const auto bu = unita_.find(b);
            if (bu == unita_.end() || bu->second.distrutta || bu->second.persa || (bu->second.flag & 0x20) || parte(bu->second) == lato) continue;
            VoceQuadro v;
            v.bersaglio = b;
            v.fonte = r.r.da;
            v.t_dato = r.r.t_fix;
            v.sensore = "radio";
            v.categoria = r.r.categoria;
            v.classe = r.r.classe;
            v.origine = r.r.origine;
            v.rilevamento = r.r.rilevamento;
            if (r.posizione_valida && r.r.pos) {
                v.posizione = true;
                v.pos = *r.r.pos;
                v.semiasse_maggiore_m = v.semiasse_minore_m = r.raggio_m;
                proponi(3, v);
            } else {
                proponi(4, v);
            }
        }
    }
    std::vector<VoceQuadro> out;
    out.reserve(meglio.size());
    for (auto& [b, c] : meglio) {
        for (const int f : fonti[b])
            if (f != c.v.fonte) c.v.altre_fonti.push_back(f);
        out.push_back(std::move(c.v));
    }
    return out;
}

}  // namespace m688
