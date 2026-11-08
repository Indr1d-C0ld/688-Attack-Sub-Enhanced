#include "nucleo/statistiche_sandbox.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <set>

#include "nucleo/geometria.hpp"

namespace m688 {

namespace {

constexpr double BUCO_S = 30.0;        // fra due campioni piu' lontani di cosi' non si integra (come l'interpolazione)
constexpr double NODO_M_S = 0.514444;
constexpr double CONTRO_S = 60.0;      // una scoperta subita entro un minuto da un proprio ping e' una contro-scoperta
constexpr double CONTRO_RITORNO_S = 30.0;   // un contatto che torna prima non e' una scoperta: l'ESM sente il radar a ogni giro (come ASSENZA_PERSO_S della traccia)

struct InfoUnita {
    std::string classe, categoria;
    int flag = 0, parte = 0;
    bool arma = false, esca = false;
    const std::vector<std::pair<double, StatoUnita>>* campioni = nullptr;
};

bool e_esca(const StatoUnita& u) {
    const std::string& c = u.classe;
    return (u.flag & 0x80) || c.find("esca") != std::string::npos || c.find("adc") != std::string::npos ||
           c.find("mg74") != std::string::npos || c.find("noisemaker") != std::string::npos;
}

// la somma degli intervalli fra campioni in cui vale `vero` (il valore del campione vale fino al prossimo)
template <class F>
double integra(const std::vector<std::pair<double, StatoUnita>>& c, F vero) {
    double s = 0.0;
    for (std::size_t i = 0; i + 1 < c.size(); ++i) {
        const double dt = c[i + 1].first - c[i].first;
        if (dt > 0.0 && dt <= BUCO_S && !c[i].second.rimossa && vero(c[i].second)) s += dt;
    }
    return s;
}

const StatoUnita* campione_a(const std::vector<std::pair<double, StatoUnita>>& c, double t) {
    const StatoUnita* u = nullptr;
    for (const auto& [ti, s] : c) {
        if (ti > t + 1e-9) break;
        u = &s;
    }
    return u;
}

double media(const std::vector<double>& v) {
    if (v.empty()) return -1.0;
    double s = 0.0;
    for (const double x : v) s += x;
    return s / static_cast<double>(v.size());
}

}  // namespace

// 0x18: la parte del luogo (M7, schieramento = 'luogo'), una terza parte armata dell'Enhanced
int parte_da_flag(int flag) {
    const int l = flag & 0x18;
    return l == 0x08 ? 0x08 : l == 0x10 ? 0x10 : l == 0x18 ? 0x18 : 0;
}

// NATO e Patto fra loro; il luogo con l'una e con l'altra (la traccia non ha la situazione di ogni coppia: e' la stima
// di una parte armata che si scontra con qualcuno)
bool parti_ostili(int a, int b) {
    return (a == 0x08 && b == 0x10) || (a == 0x10 && b == 0x08) || (a == 0x18 && (b == 0x08 || b == 0x10)) ||
           (b == 0x18 && (a == 0x08 || a == 0x10));
}

const char* nome_esito(ArmaLanciata::Esito e) {
    switch (e) {
        case ArmaLanciata::Esito::a_segno: return "a_segno";
        case ArmaLanciata::Esito::uccisione: return "uccisione";
        case ArmaLanciata::Esito::esca: return "esca";
        case ArmaLanciata::Esito::mancata: return "mancata";
        case ArmaLanciata::Esito::esaurita: return "esaurita";
        case ArmaLanciata::Esito::in_corsa: return "in_corsa";
    }
    return "?";
}

StatisticheSandbox statistiche_sandbox(const Traccia& t) {
    StatisticheSandbox out;
    out.durata_s = t.durata;
    out.complete = !t.eventi_ia.empty();
    if (const auto it = t.metadati.find("giocatore"); it != t.metadati.end()) out.giocatore = std::atoi(it->second.c_str());
    for (const EventoTraccia& e : t.registro)   // l'esito: l'evento di fine del registro
        if (e.tipo == "fine") {
            out.esito = e.codice;
            out.esito_testo = e.testo_it;
        }
    const int giocatore = out.giocatore;

    // --- le unita' ---
    std::map<int, InfoUnita> info;
    for (const auto& [id, c] : t.unita) {
        if (c.empty()) continue;
        const StatoUnita& u = c.front().second;
        InfoUnita& x = info[id];
        x.classe = u.classe;
        x.categoria = u.categoria;
        x.flag = u.flag;
        x.arma = u.categoria == "arma";
        x.esca = x.arma && e_esca(u);
        x.campioni = &c;
    }
    const auto parte_di = [&](int id) {
        const auto it = info.find(id);
        if (it == info.end()) return 0;
        if (it->second.arma) {   // un'arma e' della parte di chi l'ha lanciata
            const int l = it->second.campioni->front().second.lanciatore;
            if (l >= 0 && l != id) if (const auto jt = info.find(l); jt != info.end()) return parte_da_flag(jt->second.flag);
        }
        return parte_da_flag(it->second.flag);
    };
    const auto e_arma = [&](int id) { const auto it = info.find(id); return it != info.end() && it->second.arma; };
    const auto lanciatore_di = [&](int id) {
        const auto it = info.find(id);
        return it == info.end() || it->second.campioni->empty() ? -1 : it->second.campioni->front().second.lanciatore;
    };
    for (auto& [id, x] : info) x.parte = parte_di(id);
    const auto chi = [&](int unita) { return unita < 0 ? giocatore : unita; };   // gli eventi del giocatore con -1

    // --- catene di scoperta ---
    std::map<std::pair<int, int>, CatenaScoperta> catene;
    const auto catena = [&](int o, int b) -> CatenaScoperta* {
        if (o < 0 || b < 0 || o == b || e_arma(o) || e_arma(b) || !info.count(o) || !info.count(b)) return nullptr;
        if (info[o].parte == info[b].parte) return nullptr;
        CatenaScoperta& c = catene[{o, b}];
        c.osservatore = o;
        c.bersaglio = b;
        return &c;
    };
    for (const EventoIa& e : t.eventi_ia) {
        CatenaScoperta* c = e.tipo.rfind("contatto_", 0) == 0 || e.tipo == "possibile" ? catena(e.unita, e.altra) : nullptr;
        if (!c) continue;
        if (e.tipo == "contatto_nuovo" && c->t_contatto < 0.0) {
            c->t_contatto = e.t;
            c->sensore = e.s;
            c->distanza_m = e.a;
        } else if (e.tipo == "contatto_solido" && c->t_solido < 0.0) {
            c->t_solido = e.t;
        } else if (e.tipo == "possibile" && c->t_possibile < 0.0) {
            c->t_possibile = e.t;
        }
    }
    if (!out.complete)   // tracce senza la raccolta: le prime scoperte del registro
        for (const EventoTraccia& e : t.registro) {
            const int o = e.tipo == "scoperta_ia" ? e.unita : e.tipo == "scoperta" ? giocatore : -1;
            const int b = e.tipo == "scoperta_ia" ? e.altra : e.unita;
            if (o < 0) continue;
            if (CatenaScoperta* c = catena(o, b); c && c->t_contatto < 0.0) {
                c->t_contatto = e.tempo;
                c->distanza_m = e.distanza_m;
            }
        }
    for (const EventoTraccia& e : t.registro) {   // la prima classificazione dell'osservatore
        const int o = e.tipo == "classificazione_ia" ? e.unita : e.tipo == "classificazione" ? giocatore : -1;
        const int b = e.tipo == "classificazione_ia" ? e.altra : e.unita;
        if (o < 0) continue;
        const auto it = catene.find({o, b});
        if (it == catene.end() || it->second.classificato) continue;
        it->second.classificato = true;
        it->second.t_classificato = e.tempo;
        it->second.classe_giusta = e.giusta;
    }
    for (auto& [k, c] : catene)
        if (c.t_contatto >= 0.0 || c.t_possibile >= 0.0) out.catene.push_back(c);

    // --- TMA ---
    std::map<std::pair<int, int>, TmaUnita> tma;
    std::map<std::pair<int, int>, double> inizio_tma;
    for (const EventoTraccia& e : t.registro) {
        if (e.tipo != "tma_ia") continue;
        TmaUnita& x = tma[{e.unita, e.altra}];
        x.unita = e.unita;
        x.bersaglio = e.altra;
        x.gambe = std::max(x.gambe, e.livello);
        inizio_tma.try_emplace({e.unita, e.altra}, e.tempo);
        if (e.codice == 1 && x.soluzione_s < 0.0) {
            const auto c = catene.find({e.unita, e.altra});
            const double da = c != catene.end() && c->second.t_contatto >= 0.0 ? c->second.t_contatto : inizio_tma[{e.unita, e.altra}];
            x.soluzione_s = e.tempo - da;
        }
        if (e.codice == 2) x.rinuncia = true;
    }

    // --- armi ---
    std::map<int, int> bersaglio_lancio;   // arma -> bersaglio
    std::map<int, const EventoIa*> lancio_r3;   // arma -> il lancio con la stima (R3 del runtime)
    for (const EventoIa& e : t.eventi_ia)
        if (e.tipo == "lancio" && e.n >= 0) {
            lancio_r3.try_emplace(e.n, &e);
            if (e.altra >= 0) bersaglio_lancio.try_emplace(e.n, e.altra);
        }
    for (const EventoTraccia& e : t.registro)
        if ((e.tipo == "lancio_ia" || e.tipo == "lancio_giocatore" || e.tipo == "lancio_nemico") && e.arma >= 0)
            bersaglio_lancio.try_emplace(e.arma, e.altra);
    std::map<int, std::vector<const EventoTraccia*>> colpi_su;   // vittima -> colpi in ordine
    std::map<int, double> affondata;                           // unita' -> quando
    for (const EventoTraccia& e : t.registro) {
        if (e.tipo == "colpo") colpi_su[e.unita].push_back(&e);
        if (e.tipo == "affondamento") affondata.try_emplace(e.unita, e.tempo);
    }
    std::map<int, std::set<std::string>> siluro;   // arma -> eventi del modello
    std::map<int, int> sedotta_da;                 // arma -> esca che l'ha presa
    for (const EventoIa& e : t.eventi_ia)
        if (e.tipo == "siluro") {
            siluro[e.unita].insert(e.s);
            if (e.s == "esca_sedotta") sedotta_da.try_emplace(e.unita, e.n);
        }
    for (const auto& [id, x] : info) {
        if (!x.arma || x.esca) continue;
        const auto& c = *x.campioni;
        ArmaLanciata a;
        a.arma = id;
        a.lanciatore = c.front().second.lanciatore;
        a.classe = x.classe;
        a.t_lancio = c.front().first;
        a.durata_s = c.back().first - c.front().first;
        a.filo_s = integra(c, [](const StatoUnita& u) { return u.collegata >= 0; });
        if (const auto it = bersaglio_lancio.find(id); it != bersaglio_lancio.end()) a.bersaglio = it->second;
        // a segno: il primo colpo di quest'arma; uccisione se la vittima affonda e l'ultimo colpo prima e' suo
        const EventoTraccia* colpo = nullptr;
        for (const auto& [v, lista] : colpi_su)
            for (const EventoTraccia* k : lista)
                if (k->arma == id && (!colpo || k->tempo < colpo->tempo)) colpo = k;
        if (colpo) {
            a.vittima = colpo->unita;
            a.esito = ArmaLanciata::Esito::a_segno;
            if (const auto af = affondata.find(a.vittima); af != affondata.end() && af->second >= colpo->tempo - 1e-6) {
                const EventoTraccia* ultimo = nullptr;
                for (const EventoTraccia* k : colpi_su[a.vittima])
                    if (k->tempo <= af->second + 1e-6) ultimo = k;
                if (ultimo && ultimo->arma == id) a.esito = ArmaLanciata::Esito::uccisione;
            }
        } else if (sedotta_da.count(id)) {
            a.esito = ArmaLanciata::Esito::esca;
        } else if (siluro[id].count("esaurito")) {
            a.esito = ArmaLanciata::Esito::esaurita;
        } else if (!c.back().second.rimossa && c.back().first >= t.durata - 1e-6) {
            a.esito = ArmaLanciata::Esito::in_corsa;
        }
        out.armi.push_back(a);
    }

    // le armi senza un'unita' (cariche di profondita' e razzi: ATTACK; i missili di ATTACK_RND): un attacco per colpo del
    // registro, a segno se fa danno (una carica fallita esplode lo stesso, con danno 0), uccisione se e' l'ultimo colpo
    // prima dell'affondamento della vittima
    for (const auto& [v, lista] : colpi_su)
        for (const EventoTraccia* k : lista) {
            if (k->arma >= 0) continue;
            ArmaLanciata a;
            a.lanciatore = k->altra;
            a.bersaglio = v;
            a.classe = k->classe_arma.empty() ? "carica" : k->classe_arma;
            a.t_lancio = k->tempo;
            a.esito = k->valore > 0.0 ? ArmaLanciata::Esito::a_segno : ArmaLanciata::Esito::mancata;
            if (a.esito == ArmaLanciata::Esito::a_segno) a.vittima = v;
            if (const auto af = affondata.find(v); af != affondata.end() && k->tempo <= af->second + 1e-6) {
                const EventoTraccia* ultimo = nullptr;
                for (const EventoTraccia* x : lista)
                    if (x->tempo <= af->second + 1e-6) ultimo = x;
                if (ultimo == k) {
                    a.esito = ArmaLanciata::Esito::uccisione;
                    a.vittima = v;
                }
            }
            out.armi.push_back(a);
        }

    // --- l'errore della soluzione al lancio: la stima del lanciatore (R3) contro la verita' ---
    for (const ArmaLanciata& a : out.armi) {
        const auto l = lancio_r3.find(a.arma);
        if (l == lancio_r3.end() || l->second->m != 1 || a.bersaglio < 0) continue;
        const auto bi = t.unita.find(a.bersaglio);
        if (bi == t.unita.end()) continue;
        const StatoUnita* b = campione_a(bi->second, a.t_lancio);
        if (!b) continue;
        TmaUnita& x = tma[{a.lanciatore, a.bersaglio}];
        x.unita = a.lanciatore;
        x.bersaglio = a.bersaglio;
        if (x.errore_distanza_m) continue;   // il primo lancio su quel bersaglio
        const EventoIa& e = *l->second;
        x.errore_distanza_m = distanza({e.a, e.b}, b->pos);   // l'errore della posizione stimata
        if (e.c >= 0.0) x.errore_rotta_gradi = std::fabs(normalizza_180(b->rotta - e.c));
        if (e.d >= 0.0) x.errore_velocita_kn = std::fabs(b->velocita_kn - e.d);
    }
    // il giocatore senza R3: la soluzione proposta dalla stazione (l'errore della distanza)
    for (const ArmaLanciata& a : out.armi) {
        if (a.lanciatore != giocatore || giocatore < 0 || a.bersaglio < 0) continue;
        const Traccia::Quadro* q = nullptr;
        for (const Traccia::Quadro& x : t.quadri) {
            if (x.t > a.t_lancio + 1e-6) break;
            q = &x;
        }
        if (!q || !q->app.soluzione) continue;
        const auto gi = t.unita.find(giocatore), bi = t.unita.find(a.bersaglio);
        if (gi == t.unita.end() || bi == t.unita.end()) continue;
        const StatoUnita* g = campione_a(gi->second, a.t_lancio);
        const StatoUnita* b = campione_a(bi->second, a.t_lancio);
        if (!g || !b) continue;
        TmaUnita& x = tma[{giocatore, a.bersaglio}];
        x.unita = giocatore;
        x.bersaglio = a.bersaglio;
        if (x.errore_distanza_m) continue;   // il primo lancio su quel bersaglio
        x.errore_distanza_m = std::fabs(distanza(g->pos, b->pos) - q->app.proposta.distanza_m);
        x.errore_rotta_gradi = std::fabs(normalizza_180(b->rotta - q->app.proposta.rotta));
        x.errore_velocita_kn = std::fabs(b->velocita_kn - q->app.proposta.velocita_m_s / NODO_M_S);
    }
    for (auto& [k, x] : tma) out.tma.push_back(x);

    // --- reazioni ---
    for (const auto& [id, x] : info) {
        if (x.arma) continue;
        ReazioneUnita r;
        r.unita = id;
        for (const CatenaScoperta& c : out.catene)
            if (c.osservatore == id && c.t_contatto >= 0.0 && parti_ostili(x.parte, info[c.bersaglio].parte) &&
                (r.t_scoperta < 0.0 || c.t_contatto < r.t_scoperta)) {
                r.t_scoperta = c.t_contatto;
                r.su = c.bersaglio;
            }
        // la minaccia: il primo contatto su un'arma lanciata da un ostile
        for (const EventoIa& e : t.eventi_ia)
            if (e.tipo == "contatto_nuovo" && e.unita == id && e_arma(e.altra) && !info[e.altra].esca &&
                parti_ostili(x.parte, info[e.altra].parte)) {   // la parte di un'arma e' quella del lanciatore
                r.minaccia_t = e.t;
                break;
            }
        if (r.t_scoperta < 0.0 && r.minaccia_t < 0.0) continue;
        for (const EventoTraccia& e : t.registro) {
            if (r.t_scoperta >= 0.0 && e.tempo >= r.t_scoperta - 1e-6) {
                if (r.allerta_s < 0.0 && e.tipo == "allerta_forza" && e.livello > static_cast<int>(e.valore) &&
                    (e.unita == id || (e.unita < 0 && e.codice == x.parte)))
                    r.allerta_s = e.tempo - r.t_scoperta;
                const bool ingaggio = (e.tipo == "tma_ia" || e.tipo == "ingaggio_ia") && e.unita == id;
                const bool comandante = e.tipo == "comandante_ia" && id == giocatore &&
                                        (e.classe_arma == "tma" || e.classe_arma == "attacco");
                if (r.ingaggio_s < 0.0 && (ingaggio || comandante)) r.ingaggio_s = e.tempo - r.t_scoperta;
                if (r.lancio_s < 0.0 && (e.tipo == "lancio_ia" || e.tipo == "lancio_giocatore") && chi(e.unita) == id)
                    r.lancio_s = e.tempo - r.t_scoperta;
            }
            if (r.minaccia_t >= 0.0 && r.evasione_s < 0.0 && e.tempo >= r.minaccia_t - 1e-6) {   // senza R4
                const bool ia = (e.tipo == "evasione_ia" || e.tipo == "esca_ia" || e.tipo == "ritirata_ia") && e.unita == id;
                const bool comandante = e.tipo == "comandante_ia" && id == giocatore && e.classe_arma == "difesa";
                if (ia || comandante) r.evasione_s = e.tempo - r.minaccia_t;
            }
        }
        for (const EventoIa& e : t.eventi_ia)   // R4: le evasioni con la loro minaccia
            if (e.tipo == "evasione" && e.unita == id && r.minaccia_t >= 0.0 && e.t >= r.minaccia_t - 1e-6) {
                const double d = e.t - r.minaccia_t;
                if (r.evasione_s < 0.0 || d < r.evasione_s) r.evasione_s = d;
                break;
            }
        out.reazioni.push_back(r);
    }

    // --- esposizione ---
    bool con_emissioni = false;
    for (const EventoIa& e : t.eventi_ia) con_emissioni = con_emissioni || e.tipo == "emissione";
    for (const auto& [id, x] : info) {
        if (x.arma) continue;
        EsposizioneUnita s;
        s.unita = id;
        const auto& c = *x.campioni;
        s.schiera_s = integra(c, [](const StatoUnita& u) { return (u.alberi & 0x01) != 0; });
        s.periscopio_s = integra(c, [](const StatoUnita& u) { return (u.alberi & 0x02) != 0; });
        s.antenna_s = integra(c, [](const StatoUnita& u) { return (u.alberi & 0x04) != 0; });
        if (!con_emissioni) s.radar_s = integra(c, [](const StatoUnita& u) { return (u.emissioni & StatoUnita::EMESSO_RADAR) != 0; });
        std::vector<double> ping;   // gli istanti dei propri ping (per le contro-scoperte)
        if (con_emissioni) {   // R5: ping, radar e radio di tutti
            for (const EventoIa& e : t.eventi_ia) {
                if (e.tipo != "emissione" || e.unita != id) continue;
                if (e.s == "ping") ++s.ping, ping.push_back(e.t);
                else if (e.s == "radio") ++s.radio;
                else if (e.s == "radar") {   // acceso: la durata alla chiusura, o fino alla fine
                    double d = e.a;
                    if (d < 0.0) {
                        d = std::max(0.0, t.durata - e.t);
                        for (const EventoIa& f : t.eventi_ia)
                            if (f.tipo == "emissione_fine" && f.unita == id && f.s == "radar" && f.t >= e.t - 1e-6) {
                                d = f.a;
                                break;
                            }
                    }
                    s.radar_s += d;
                }
            }
        } else {
            for (const EventoTraccia& e : t.registro) {
                if (e.tipo == "ping_attivo" && chi(e.unita) == id) ++s.ping, ping.push_back(e.tempo);
                if ((e.tipo == "trasmissione" && chi(e.unita) == id) || (e.tipo == "messaggio_ia" && e.unita == id)) ++s.radio;
            }
        }
        std::map<int, double> perso_da;   // l'ultimo perso di chi la sentiva
        for (const EventoIa& e : t.eventi_ia) {
            if (e.tipo == "contatto_perso" && e.altra == id) perso_da[e.unita] = e.t;
            if (e.tipo == "contatto_nuovo" && e.altra == id && !e_arma(e.unita) && info.count(e.unita) &&
                info[e.unita].parte != x.parte) {
                if (const auto p = perso_da.find(e.unita); p != perso_da.end() && e.t - p->second < CONTRO_RITORNO_S) continue;
                const StatoUnita* u = campione_a(c, e.t);
                bool emetteva = u && (u->emissioni != 0);
                for (const double p : ping) emetteva = emetteva || (e.t >= p - 1e-6 && e.t - p <= CONTRO_S);
                if (e.s == "esm" || emetteva) ++s.contro_scoperte;
            }
        }
        if (s.periscopio_s > 0.0 || s.antenna_s > 0.0 || s.schiera_s > 0.0 || s.radar_s > 0.0 || s.ping || s.radio ||
            s.contro_scoperte)
            out.esposizione.push_back(s);
    }

    // --- navigazione ---
    for (const auto& [id, x] : info) {
        if (x.arma) continue;
        const auto& c = *x.campioni;
        NavigazioneUnita n;
        n.unita = id;
        double somma = 0.0, tempo = 0.0;
        bool stima = false;
        for (std::size_t i = 0; i < c.size(); ++i) {
            const StatoUnita& u = c[i].second;
            if (u.rimossa) continue;
            const double e = distanza(u.pos_stimata, u.pos);
            if (e > 1.0 || u.sigma_nav_m > 0.0) stima = true;
            n.errore_max_m = std::max(n.errore_max_m, e);
            if (i + 1 < c.size()) {
                const double dt = c[i + 1].first - c[i].first;
                if (dt > 0.0 && dt <= BUCO_S) somma += e * dt, tempo += dt;
            }
        }
        for (const EventoTraccia& e : t.registro)
            if (e.tipo == "punto_nave" && chi(e.unita) == id) (e.giusta ? n.punti_nave : n.punti_mancati) += 1;
        for (const EventoIa& e : t.eventi_ia)   // R6: i punti nave dell'IA
            if (e.tipo == "punto_nave" && e.unita == id && id != giocatore) (e.m ? n.punti_nave : n.punti_mancati) += 1;
        if (!stima && !n.punti_nave && !n.punti_mancati) continue;
        n.errore_medio_m = tempo > 0.0 ? somma / tempo : n.errore_max_m;
        out.navigazione.push_back(n);
    }

    // --- per parte ---
    std::map<int, std::vector<double>> ritardi, distanze, reazioni, ingaggi, soluzioni, errori;
    for (const auto& [id, x] : info) {
        if (x.arma) continue;
        StatParte& p = out.parti[x.parte];
        p.parte = x.parte;
        ++p.unita;
    }
    for (const auto& [v, tf] : affondata) {
        if (!info.count(v) || info[v].arma) continue;
        out.parti[info[v].parte].perse += 1;
        const EventoTraccia* ultimo = nullptr;   // a chi va l'uccisione: l'ultimo colpo prima dell'affondamento
        for (const EventoTraccia* k : colpi_su[v])
            if (k->tempo <= tf + 1e-6) ultimo = k;
        if (!ultimo) continue;
        const int lanciatore = ultimo->altra >= 0 ? ultimo->altra : lanciatore_di(ultimo->arma);
        if (lanciatore >= 0 && info.count(lanciatore) && info[lanciatore].parte != info[v].parte)
            out.parti[info[lanciatore].parte].inflitte += 1;
    }
    for (const CatenaScoperta& c : out.catene) {
        if (c.t_contatto < 0.0) continue;
        const int po = info[c.osservatore].parte, pb = info[c.bersaglio].parte;
        if (!parti_ostili(po, pb)) continue;
        out.parti[po].scoperte += 1;
        out.parti[pb].scoperte_subite += 1;
        distanze[po].push_back(c.distanza_m);
        if (c.ritardo_s() >= 0.0) ritardi[po].push_back(c.ritardo_s());
    }
    for (const ArmaLanciata& a : out.armi) {
        const int p = a.arma >= 0 ? parte_di(a.arma) : parte_di(a.lanciatore);
        StatParte& s = out.parti[p];
        s.parte = p;
        ++s.lanci;
        auto& k = s.pk[a.classe];
        ++k.second;
        if (a.esito == ArmaLanciata::Esito::a_segno || a.esito == ArmaLanciata::Esito::uccisione) {
            ++s.a_segno;
            ++k.first;
        }
    }
    for (const auto& [id, x] : info)
        if (x.esca) out.parti[x.parte].esche += 1;
    for (const auto& [arma, esca] : sedotta_da)
        if (info.count(esca)) out.parti[info[esca].parte].esche_efficaci += 1;
    std::map<int, std::vector<double>> lanci_s, evasioni, gambe, errori_nav;
    for (const ReazioneUnita& r : out.reazioni) {
        const int p = info[r.unita].parte;
        if (r.allerta_s >= 0.0) reazioni[p].push_back(r.allerta_s);
        if (r.ingaggio_s >= 0.0) ingaggi[p].push_back(r.ingaggio_s);
        if (r.lancio_s >= 0.0) lanci_s[p].push_back(r.lancio_s);
        if (r.evasione_s >= 0.0) evasioni[p].push_back(r.evasione_s);
    }
    std::map<int, double> periscopio, antenna, radar;
    for (const EsposizioneUnita& e : out.esposizione) {
        const int p = info[e.unita].parte;
        StatParte& s = out.parti[p];
        periscopio[p] += e.periscopio_s;
        antenna[p] += e.antenna_s;
        radar[p] += e.radar_s;
        s.ping += e.ping;
        s.radio += e.radio;
        s.contro_scoperte += e.contro_scoperte;
    }
    for (const NavigazioneUnita& n : out.navigazione) {
        const int p = info[n.unita].parte;
        StatParte& s = out.parti[p];
        errori_nav[p].push_back(n.errore_medio_m);
        s.errore_nav_max_m = std::max(s.errore_nav_max_m, n.errore_max_m);
        s.punti_nave += n.punti_nave;
    }
    for (const TmaUnita& x : out.tma) {
        if (!info.count(x.unita)) continue;
        const int p = info[x.unita].parte;
        if (x.gambe > 0) gambe[p].push_back(x.gambe);
        if (x.soluzione_s >= 0.0) soluzioni[p].push_back(x.soluzione_s);
        if (x.errore_distanza_m) errori[p].push_back(*x.errore_distanza_m);
    }
    for (auto& [p, s] : out.parti) {
        s.parte = p;
        s.scambio = s.perse > 0 ? static_cast<double>(s.inflitte) / s.perse : s.inflitte;
        s.ritardo_medio_s = media(ritardi[p]);
        s.distanza_media_m = media(distanze[p]);
        s.reazione_media_s = media(reazioni[p]);
        s.ingaggio_medio_s = media(ingaggi[p]);
        s.soluzione_media_s = media(soluzioni[p]);
        s.errore_lancio_medio_m = media(errori[p]);
        s.lancio_medio_s = media(lanci_s[p]);
        s.evasione_media_s = media(evasioni[p]);
        s.gambe_medie = media(gambe[p]);
        s.errore_nav_medio_m = media(errori_nav[p]);
        if (s.unita > 0) {
            s.periscopio_medio_s = periscopio[p] / s.unita;
            s.antenna_media_s = antenna[p] / s.unita;
            s.radar_medio_s = radar[p] / s.unita;
        }
    }
    return out;
}

std::map<std::string, double> numeri_parte(const StatParte& p) {
    return {{"unita", p.unita},
            {"perse", p.perse},
            {"inflitte", p.inflitte},
            {"scambio", p.scambio},
            {"scoperte", p.scoperte},
            {"scoperte_subite", p.scoperte_subite},
            {"ritardo_medio_s", p.ritardo_medio_s},
            {"distanza_media_m", p.distanza_media_m},
            {"lanci", p.lanci},
            {"a_segno", p.a_segno},
            {"esche", p.esche},
            {"esche_efficaci", p.esche_efficaci},
            {"reazione_media_s", p.reazione_media_s},
            {"ingaggio_medio_s", p.ingaggio_medio_s},
            {"soluzione_media_s", p.soluzione_media_s},
            {"errore_lancio_medio_m", p.errore_lancio_medio_m},
            {"lancio_medio_s", p.lancio_medio_s},
            {"evasione_media_s", p.evasione_media_s},
            {"gambe_medie", p.gambe_medie},
            {"periscopio_medio_s", p.periscopio_medio_s},
            {"antenna_media_s", p.antenna_media_s},
            {"radar_medio_s", p.radar_medio_s},
            {"ping", p.ping},
            {"radio", p.radio},
            {"contro_scoperte", p.contro_scoperte},
            {"errore_nav_medio_m", p.errore_nav_medio_m},
            {"errore_nav_max_m", p.errore_nav_max_m},
            {"punti_nave", p.punti_nave}};
}

namespace {

// il quantile q (0..1) di valori ordinati, per interpolazione lineare fra i due vicini
double quantile(const std::vector<double>& ordinati, double q) {
    if (ordinati.empty()) return 0.0;
    const double x = q * static_cast<double>(ordinati.size() - 1);
    const auto i = static_cast<std::size_t>(x);
    if (i + 1 >= ordinati.size()) return ordinati.back();
    return ordinati[i] + (x - static_cast<double>(i)) * (ordinati[i + 1] - ordinati[i]);
}

}  // namespace

RiassuntoSerie riassunto_partite(const std::vector<StatisticheSandbox>& v) {
    // i nomi che valgono -1 quando mancano (medie senza dati); gli altri sono conteggi, sempre validi
    static const std::set<std::string> FACOLTATIVI = {
        "ritardo_medio_s", "distanza_media_m", "reazione_media_s", "ingaggio_medio_s", "soluzione_media_s",
        "errore_lancio_medio_m", "lancio_medio_s", "evasione_media_s", "gambe_medie", "periscopio_medio_s",
        "antenna_media_s", "radar_medio_s", "errore_nav_medio_m", "errore_nav_max_m"};
    RiassuntoSerie out;
    out.partite = static_cast<int>(v.size());
    std::map<int, std::map<std::string, std::vector<double>>> valori;
    for (const StatisticheSandbox& s : v) {
        EsitoSerie& e = out.esiti[s.esito];
        ++e.volte;
        if (e.testo.empty()) e.testo = s.esito >= 0 ? s.esito_testo : "non finita";
        for (const auto& [p, x] : s.parti) {
            for (const auto& [nome, val] : numeri_parte(x))
                if (!(FACOLTATIVI.count(nome) && val < 0.0)) valori[p][nome].push_back(val);
            for (const auto& [classe, k] : x.pk) {
                auto& somma = out.pk[p][classe];
                somma.first += k.first;
                somma.second += k.second;
            }
        }
    }
    for (auto& [codice, e] : out.esiti) e.frequenza = out.partite > 0 ? static_cast<double>(e.volte) / out.partite : 0.0;
    for (auto& [p, m] : valori)
        for (auto& [nome, xs] : m) {
            ValoreSerie r;
            r.n = static_cast<int>(xs.size());
            r.media = media(xs);
            double q = 0.0;
            for (const double x : xs) q += (x - r.media) * (x - r.media);
            r.dev = xs.size() > 1 ? std::sqrt(q / static_cast<double>(xs.size() - 1)) : 0.0;
            std::vector<double> ordinati = xs;   // una copia: l'ordine delle partite non si tocca
            std::sort(ordinati.begin(), ordinati.end());
            r.min = ordinati.front();
            r.max = ordinati.back();
            r.q10 = quantile(ordinati, 0.1);
            r.mediana = quantile(ordinati, 0.5);
            r.q90 = quantile(ordinati, 0.9);
            out.parti[p][nome] = r;
        }
    return out;
}

std::map<int, std::map<std::string, ValoreSerie>> riassunto_serie(const std::vector<StatisticheSandbox>& v) {
    return riassunto_partite(v).parti;
}

}  // namespace m688
