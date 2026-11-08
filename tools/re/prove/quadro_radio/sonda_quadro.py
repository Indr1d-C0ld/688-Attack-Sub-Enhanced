# copie di missione_forze.cpp e missione.cpp che scrivono su stderr le righe QUADRO: salite, chiusure, copie, dato usato, che cosa
# ferma alle finestre, i cicli col filo virtuale, i rapporti, le occasioni di scoperta (proposta.md §9.4, §9.8). Stessa partita,
# solo righe in piu': niente numeri casuali, niente stato della missione. Per la misura; fuori da engine/
# Uso: sonda_quadro.py [cartella di uscita]
import pathlib, sys
R = pathlib.Path('/home/randolph/Claude/melchior-688')
U = pathlib.Path(sys.argv[1]) if len(sys.argv) > 1 else R / 'engine/build_radio/sonda_scena'
t = (R / 'engine/missioni/src/missione_forze.cpp').read_text(encoding='utf-8')
def r(a, b):
    global t
    assert t.count(a) == 1, a
    t = t.replace(a, b)
t = '''#include <cstdio>
#include <map>
#include <set>
#include <string>
namespace {
struct FiloVirtuale { double filo_da = -1.0, oltre = 0.0, cieco_fino = -1.0, dentro_da = -1.0, rotta = -1.0, ciclo_visto = -1.0; bool rotto = false, meta = false; int a = -1; };
std::map<int, FiloVirtuale> sonda_filo;
}
''' + t
# il rapporto del battello: la trasmissione al teatro
r('''                F.messaggi.push_back({tempo_, arrivo, id, bid, -1, MessaggioIa::Canale::teatro, perso});
''', '''                if (sub) std::fprintf(stderr, "QUADRO rapporto t=%.0f id=%d su=%d a=%d antenna=%d quota=%.1f lato=%d perso=%d\\n", tempo_, id, bid, allerta_unita(id).value_or(0), a_quota_antenna(o) && !sotto_banchisa(id), o.quota, lo, perso);
                F.messaggi.push_back({tempo_, arrivo, id, bid, -1, MessaggioIa::Canale::teatro, perso});
''')
# la copia
r('            if (copia(x.ricevente, *v)) consegna_rapporto(x.ricevente, x.r);\n',
  '''            if (copia(x.ricevente, *v)) {
                const auto* bq = trova(x.r.su);
                const int lq = bq ? lato_creduto(*v, *bq) : -1;
                std::fprintf(stderr, "QUADRO copia t=%.0f id=%d su=%d a=%d ost=%d\\n", tempo_, x.ricevente, x.r.su, allerta_unita(x.ricevente).value_or(0), bq && lq > 0 && lq != lato_di(*v) && x.r.categoria != "aereo" && !((bq->flag & 3) == 3 || bq->categoria == "aereo"));
                consegna_rapporto(x.ricevente, x.r);
            }
''')
# il FLASH in onda: il campanello che suonerebbe
r('''                nuove.push_back({prossimo, k, x.lato, x.r, true});
''', '''                if (x.r.livello >= 1 && x.r.categoria == "sottomarino") {
                    const auto sv = sonda_filo.find(k);
                    std::fprintf(stderr, "QUADRO flash t=%.0f id=%d su=%d lato=%d a=%d quota=%.1f vel=%.1f rotto=%d\\n", tempo_, k, x.r.su, x.lato, allerta_unita(k).value_or(0), v.quota, std::fabs(v.velocita), sv != sonda_filo.end() && sv->second.rotto);
                }
                nuove.push_back({prossimo, k, x.lato, x.r, true});
''')
r('            R.ciclo = R.ciclo_tentato = prossimo;\n            R.quota_prima = u.quota_ordinata;\n',
  '            std::fprintf(stderr, "QUADRO salita t=%.0f id=%d quota=%.0f vel=%.1f prossimo=%.0f a=%d rotto=%d\\n", tempo_, id, u.quota, u.velocita_ordinata, prossimo, a, sonda_filo[id].rotto);\n'
  '            R.ciclo = R.ciclo_tentato = prossimo;\n            R.quota_prima = u.quota_ordinata;\n')
r('''std::fabs(u.velocita_ordinata - R.velocita_data) > 1e-9) {
                fine_ascolto();
''', '''std::fabs(u.velocita_ordinata - R.velocita_data) > 1e-9) {
                std::fprintf(stderr, "QUADRO chiusa t=%.0f id=%d quota=%.0f script=%d occupato=%d rimanda=%d quota_altri=%d vel_altri=%d\\n", tempo_, id, u.quota, script_ora, occupato, contatto_ostile(RIMANDA_M[livello]),
                             std::fabs(u.quota_ordinata - QUOTA_ORDINE_ANTENNA_M) > 1e-9, std::fabs(u.velocita_ordinata - R.velocita_data) > 1e-9);
                fine_ascolto();
''')
r('''            reazioni_ia_.push_back({tempo_, id, R.caccia, u.categoria, "dato_radio_lasciato", perche});
''', '''            std::fprintf(stderr, "QUADRO lasciato t=%.0f id=%d su=%d perche=%s\\n", tempo_, id, R.caccia, perche);
            reazioni_ia_.push_back({tempo_, id, R.caccia, u.categoria, "dato_radio_lasciato", perche});
''')
# il dato usato, poi il filo virtuale e i cicli
r('''                reazioni_ia_.push_back({tempo_, id, R.caccia, u.categoria, "dato_radio_usato", "quadro_radio"});
            }
        }
''', '''                std::fprintf(stderr, "QUADRO usato t=%.0f id=%d su=%d a=%d\\n", tempo_, id, R.caccia, a);
                reazioni_ia_.push_back({tempo_, id, R.caccia, u.categoria, "dato_radio_usato", "quadro_radio"});
            }
        }
        {   // il filo come sarebbe (proposta.md §9.1), una volta al secondo: da A2, filato in 120 s, cieco dopo le virate, rotto oltre 20 nodi per piu' di 60 s
            auto& V = sonda_filo[id];
            if (V.a != a) { std::fprintf(stderr, "QUADRO allerta t=%.0f id=%d a=%d lato=%d\\n", tempo_, id, a, lu); V.a = a; }
            const double vel = std::fabs(u.velocita);
            if (a >= 2 && V.filo_da < 0.0 && !sotto_banchisa(id)) V.filo_da = tempo_;
            else if (a < 2) V.filo_da = -1.0;
            const bool filato = V.filo_da >= 0.0 && tempo_ - V.filo_da >= RADIO.filo_filata_s && !V.rotto;
            if (filato) {
                double dr = V.rotta < 0.0 ? 0.0 : std::fabs(u.rotta - V.rotta);
                if (dr > 180.0) dr = 360.0 - dr;
                if (dr > RADIO.filo_virata_deg_s) V.cieco_fino = tempo_ + RADIO.filo_cieco_dopo_virata_s;
                V.oltre = vel > RADIO.filo_rottura_kn ? V.oltre + 1.0 : 0.0;
                if (V.oltre > 60.0) V.rotto = true;
            }
            V.rotta = u.rotta;
            const bool nei_limiti = filato && !V.rotto && u.quota <= RADIO.quota_filo_m && vel <= RADIO.filo_meta_kn && tempo_ >= V.cieco_fino && !sotto_banchisa(id);
            if (!nei_limiti) { V.dentro_da = -1.0; V.meta = false; }
            else {
                if (V.dentro_da < 0.0) V.dentro_da = tempo_;
                V.meta = V.meta || vel > RADIO.filo_pieno_kn;
            }
            if (ciclo > 0.0 && prossimo - tempo_ <= 1.0 && V.ciclo_visto != prossimo) {
                V.ciclo_visto = prossimo;
                const int lim = nei_limiti && prossimo - V.dentro_da >= COPIA_S ? (V.meta ? 2 : 1) : 0;
                const bool libero = !occupato && !contatto_suo && !script_ora && tempo_ >= R.caccia_non_prima;
                std::string msg;
                for (const auto& x : F.in_viaggio) {
                    if (!x.broadcast || x.ricevente != id || x.t > prossimo + 2.0) continue;
                    const auto* bq = trova(x.r.su);
                    const int lq = bq ? lato_creduto(u, *bq) : -1;
                    const bool ost = bq && lq > 0 && lq != lu && x.r.categoria != "aereo" && !((bq->flag & 3) == 3 || bq->categoria == "aereo");
                    ContattoRadio cr;
                    cr.r = x.r;
                    cr.t_arrivo = prossimo;
                    const auto vk = F.d.velocita_kn.find(x.r.categoria);
                    cr.raggio_m = errore_rapporto(x.r, F.d) + (vk != F.d.velocita_kn.end() ? vk->second : 20.0) * m_s_per_nodo_ * std::max(0.0, prossimo - x.r.t_fix);
                    cr.posizione_valida = x.r.pos.has_value() && cr.raggio_m < F.d.ellisse_max_m;
                    char b[96];
                    std::snprintf(b, sizeof b, "%d:%.0f:%d:%d:%.0f,", x.r.su, x.r.t_fix, ost, ost && usabile(x.r.su, cr), cr.raggio_m);
                    msg += b;
                }
                std::fprintf(stderr, "QUADRO ciclo t=%.0f id=%d lato=%d a=%d quota=%.1f vel=%.1f lim=%d rotto=%d esposto=%d libero=%d salita=%d err=%.0f msg=%s\\n", prossimo, id, lu, a, u.quota, vel, lim, V.rotto,
                             R.esposto_da >= 0.0 && prossimo - R.esposto_da >= COPIA_S, libero, R.salita, std::hypot(e.x, e.y), msg.c_str());
            }
        }
''')
r('''        if (R.salita) {
''', '''        {   // alle finestre: che cosa lo ferma, una riga per ciclo e per motivo
            const int al = lu == 0x08 ? 0x10 : 0x08;
            if (!R.salita && !esposto && lu != 0 && !(u.flag & 0x40) && ciclo > 0.0 && situazione_di(lu, al) != Situazione::pace &&
                prossimo >= R.ultimo_ascolto + (a >= 3 ? ASCOLTO_A3_OGNI_S : ASCOLTO_OGNI_S) && prossimo - tempo_ <= u.quota / RATEO_SALITA_M_S + ASCOLTO_PRIMA_S) {
                const auto d0 = manovre_difesa_.count(id) ? 1 : 0;
                const int rz0 = rz != reazioni_in_corso_.end() && tempo_ < rz->second.fine;
                const int tm0 = tm != armi_ia_->tma.end() && tm->first.first == id;
                char s[256];
                std::snprintf(s, sizeof s, "script=%d affonda=%d difesa=%d reazione=%d tma=%d aggancio=%d missile=%d pedina=%d vetrino=%d evasione=%d rimanda=%d tentato=%d banchisa=%d", script_ora, u.affondamento > 0, d0, rz0, tm0,
                              u.agganciato >= 0, ml != missili_ia_.end() && ml->second.in_manovra, static_cast<int>(pedine_vicine_.count(id)), ordine_vetrino, guida_evasione(u).has_value(), contatto_ostile(RIMANDA_M[livello]), prossimo == R.ciclo_tentato, sotto_banchisa(id));
                static std::map<std::pair<int, double>, std::string> visti;
                auto& v = visti[{id, prossimo}];
                if (v != s) {
                    v = s;
                    double vic = 1e18;
                    for (const auto& [cid, c] : u.contatti) if (const auto* b = trova(cid); b && !arma_o_boa(*b) && ostili(u, *b) && c.qualita >= QUALITA_SOLIDA && tempo_ - c.ultima_scoperta <= CONTATTO_FRESCO_S && c.posizione_nota) vic = std::min(vic, distanza(u.pos, c.pos_stimata));
                    std::fprintf(stderr, "QUADRO finestra t=%.0f id=%d prossimo=%.0f vicino=%.0f %s\\n", tempo_, id, prossimo, vic, s);
                }
            }
        }
        if (R.salita) {
''')
# gli affondati, compreso il giocatore
r('''    for (auto& [id, u] : unita_) {
        if (u.giocatore || u.categoria != "sottomarino" || u.distrutta || u.persa || arma_o_boa(u) || u.pos.y > RISERVA_Y_M) continue;
''', '''    for (auto& [id, u] : unita_) {
        if (u.categoria == "sottomarino" && (u.distrutta || u.affondamento > 0)) {
            static std::set<int> visti;
            if (visti.insert(id).second) std::fprintf(stderr, "QUADRO affondato t=%.0f id=%d giocatore=%d lato=%d\\n", tempo_, id, u.giocatore, lato_di(u));
        }
        if (u.giocatore || u.categoria != "sottomarino" || u.distrutta || u.persa || arma_o_boa(u) || u.pos.y > RISERVA_Y_M) continue;
''')
# la trasmissione del rapporto e la radiogoniometria (P3, proposta.md §9.2). Si scrivono solo se il codice le ha
for prima, riga in [
    ('                        radiogoniometria_ia(id, RAPPORTO_UHF_S, true);\n', '                        std::fprintf(stderr, "QUADRO trasmette t=%.0f id=%d lato=%d a=%d\\n", tempo_, id, lo, allerta_unita(id).value_or(0));\n'),
    ('    reazioni_ia_.push_back({tempo_, -1, id, "rete", "localizza", "radiogoniometria"});', '    std::fprintf(stderr, "QUADRO fix t=%.0f id=%d lato=%d errore=%.0f ritardo=%.0f rapporto=%d\\n", tempo_, id, lt, errore, ritardo, rapporto);\n'),
    ('        reazioni_ia_.push_back({tempo_, esm, id, u.categoria, "localizza", "radiogoniometria"});', '        std::fprintf(stderr, "QUADRO esm t=%.0f id=%d lato=%d da=%d rapporto=%d\\n", tempo_, id, lt, esm, rapporto);\n'),
    ('            m.richiesta = q;\n', '            if (x.r.errore_m > 0.0) std::fprintf(stderr, "QUADRO fix_giocatore t=%.0f su=%d errore=%.0f\\n", tempo_, x.r.su, x.r.errore_m);\n'),
    ('        cue_asw(k, c);\n', '        if (r.errore_m > 0.0) std::fprintf(stderr, "QUADRO datum t=%.0f aereo=%d su=%d errore=%.0f\\n", tempo_, k, r.su, r.errore_m);\n'),
]:
    r(prima, riga + prima)
if '--carica' in sys.argv:
    # per capire S03 seme 1 (proposta.md §9.10): con M688_SENZA_P3=1 la regola dei rapporti di prima; una riga al secondo con
    # la carica, lo stato di ogni unita' e i numeri casuali che verrebbero dopo (su copie: la partita non cambia)
    r('if (sub && (F.quadro_sottomarini ? !a_quota_antenna(o) || sotto_banchisa(id) : !riceve(o))) continue;',
      'if (sub && (F.quadro_sottomarini && !sonda_senza_p3() ? !a_quota_antenna(o) || sotto_banchisa(id) : !riceve(o))) continue;')
    r('if (sub && F.quadro_sottomarini) {   // il battello che riferisce', 'if (sub && F.quadro_sottomarini && !sonda_senza_p3()) {   // il battello che riferisce')
    # con M688_CASO_A_PARTE=1 i rapporti dei battelli non spostano i numeri casuali di tutti gli altri: resta la sola differenza di contenuto
    r('''// un sottomarino immerso non trasmette
''', '''// un sottomarino immerso non trasmette
        struct SondaG { Casuale& a; Casuale& b; Casuale sa, sb; bool on; ~SondaG() { if (on) { a = sa; b = sb; } } } sonda_g{F.caso, F.caso_radio, F.caso, F.caso_radio, sub && std::getenv("M688_CASO_A_PARTE") != nullptr};
''')
    r('''void Missione::passo_radio_sottomarini() {
    auto& F = *forze_;
''', '''void Missione::passo_radio_sottomarini() {
    auto& F = *forze_;
    {
        const auto dopo = [](Casuale c) { return static_cast<unsigned>(c.prossimo() >> 40); };
        std::fprintf(stderr, "QUADRO mondo t=%.0f caso=%x radio=%x casuale=%x reaz=%x acu=%x son=%x\\n", tempo_, dopo(F.caso), dopo(F.caso_radio), dopo(casuale_), dopo(caso_reazioni_), dopo(caso_acustica_), dopo(caso_sonarista_));
        for (const auto& [sid, su] : unita_) {
            if (su.distrutta || su.persa || arma_o_boa(su)) continue;
            const auto bi = batterie_.find(sid);
            std::string cc;
            if (bi != batterie_.end())
                for (const auto& [oid, so] : unita_)
                    if (ostili(so, su) && !so.distrutta && !so.persa)
                        if (const auto c = so.contatti.find(sid); c != so.contatti.end() && c->second.posizione_nota)
                            cc += std::to_string(oid) + ":" + std::to_string(static_cast<int>(tempo_ - c->second.ultima_scoperta)) + ",";
            std::fprintf(stderr, "QUADRO stato t=%.0f id=%d carica=%.5f sn=%d em=%d quota=%.2f vel=%.3f vo=%.3f rotta=%.2f x=%.1f y=%.1f ev=%d ag=%d nota=%s\\n", tempo_, sid,
                         bi != batterie_.end() ? bi->second.carica : -1.0, bi != batterie_.end() && bi->second.snorkel, bi != batterie_.end() && bi->second.emersa, su.quota, su.velocita, su.velocita_ordinata, su.rotta, su.pos.x, su.pos.y,
                         guida_evasione(su).has_value(), su.agganciato, cc.c_str());
        }
    }
''')
    t = '''#include <cstdlib>
static bool sonda_senza_p3() { static const bool v = std::getenv("M688_SENZA_P3") != nullptr; return v; }
''' + t
(U / 'missione_forze_sonda.cpp').write_text(t, encoding='utf-8')

t = (R / 'engine/missioni/src/missione.cpp').read_text(encoding='utf-8')
t = '''#include <cstdio>
#include <map>
#include <string>
#include <tuple>
namespace {
// l'occasione di proposta.md §9.8: almeno 60 s con il battello esposto e il radar che puo' vederlo (o l'ESM che lo intercetta)
void sonda_occasione(double t, int id, int oss, const char* sensore, bool giocatore, bool nemico, double buco_s) {
    struct P { double da, ultimo; bool scritta; };
    static std::map<std::tuple<int, int, std::string>, P> m;
    auto [it, nuovo] = m.try_emplace({id, oss, sensore}, P{t, t, false});
    auto& p = it->second;
    if (!nuovo && t - p.ultimo > buco_s) p = P{t, t, false};
    p.ultimo = t;
    if (!p.scritta && t - p.da >= 60.0) {
        p.scritta = true;
        std::fprintf(stderr, "QUADRO occasione t=%.0f id=%d oss=%d sensore=%s giocatore=%d nemico=%d da=%.0f\\n", t, id, oss, sensore, giocatore, nemico, p.da);
    }
}
bool sonda_parte(int f) { return (f & 0x18) == 0x08 || (f & 0x18) == 0x10; }
}
''' + t
r('''    if (p_giro > 0.0) possibile(u.id, s.id, "radar", 0.0);
''', '''    if (p_giro > 0.0) possibile(u.id, s.id, "radar", 0.0);
    if (p_giro > 0.0 && !s.giocatore && sonda_parte(s.flag)) sonda_occasione(tempo_, s.id, u.id, "radar", u.giocatore, sonda_parte(u.flag) && ((u.flag ^ s.flag) & 0x18) != 0, 5.0);
''')
r('''            possibile(id, i.unita, "esm", 0.0);
''', '''            possibile(id, i.unita, "esm", 0.0);
            if (emittente->categoria == "sottomarino" && !emittente->giocatore && sonda_parte(emittente->flag)) sonda_occasione(tempo_, i.unita, id, "esm", false, sonda_parte(u.flag) && ((u.flag ^ emittente->flag) & 0x18) != 0, 30.0);
''')
r('''                possibile(g->id, i.unita, "esm", 0.0);
''', '''                possibile(g->id, i.unita, "esm", 0.0);
                if (const auto* em_s = trova(i.unita); em_s->categoria == "sottomarino" && sonda_parte(em_s->flag)) sonda_occasione(tempo_, i.unita, g->id, "esm", true, sonda_parte(g->flag) && ((g->flag ^ em_s->flag) & 0x18) != 0, 30.0);
''')
(U / 'missione_sonda.cpp').write_text(t, encoding='utf-8')
