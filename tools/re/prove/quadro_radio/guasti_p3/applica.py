# Copia guastata di missione_forze.cpp per P3: ogni guasto si accende con M688_GUASTO=<n> (albero condiviso intatto)
R='/home/randolph/Claude/melchior-688/engine/missioni/src/'
s=open(R+'missione_forze.cpp',encoding='utf-8').read()
def r(a,b):
    global s
    assert s.count(a)==1,(a,s.count(a))
    s=s.replace(a,b)
r('if (sub && (F.quadro_sottomarini ? !a_quota_antenna(o) || sotto_banchisa(id) : !riceve(o))) continue;',
  'if (sub && (F.quadro_sottomarini && !GU(201) ? !a_quota_antenna(o) || sotto_banchisa(id) : !riceve(o))) continue;')
r('Casuale& caso = rapporto ? F.caso_radio : F.caso;','Casuale& caso = rapporto && !GU(202) ? F.caso_radio : F.caso;')
r('if (F.quadro_sottomarini) rp.errore_m = errore;','if (F.quadro_sottomarini && !GU(203)) rp.errore_m = errore;')
r('c.raggio_m = errore_rapporto(c.r, forze_->d) + u','c.raggio_m = (GU(204) ? forze_->d.r0_m : errore_rapporto(c.r, forze_->d)) + u')
r('q.errore_m = errore_rapporto(x.r, F.d);','q.errore_m = GU(205) ? F.d.r0_m : errore_rapporto(x.r, F.d);')
r('if (sub && F.quadro_sottomarini) {   // il battello che riferisce','if (sub && (GU(206) || F.quadro_sottomarini) && !GU(207)) {   // il battello che riferisce')
r('if (lato != lt) F.in_viaggio.push_back({tempo_ + ritardo, -2, lato, rp});','if (lato != lt) F.in_viaggio.push_back({tempo_ + (GU(209) ? 0.0 : ritardo), -2, lato, rp});')
# il ramo fix -> aereo ASW e il limite di una trasmissione al minuto
r('if (tempo_ - R.ultima_trasmissione >= TRASMISSIONE_OGNI_S) {','if (GU(208) || tempo_ - R.ultima_trasmissione >= TRASMISSIONE_OGNI_S) {')
r('        cue_asw(k, c);\n','        if (!GU(210)) cue_asw(k, c);\n')
r('        c.raggio_m = errore_rapporto(r, F.d);\n','        c.raggio_m = GU(211) ? F.d.r0_m : errore_rapporto(r, F.d);\n')
r('(r.categoria == "sottomarino" || r.categoria == "sconosciuto")) {','(r.categoria == "sottomarino" || (r.categoria == "sconosciuto" && !GU(212)))) {')
r('        c.t = r.t_fix;\n','        c.t = GU(213) ? tempo_ : r.t_fix;\n')
r('        c.pos = *r.pos;\n','        c.pos = GU(214) ? Vec2{r.pos->x + 20000.0, r.pos->y} : *r.pos;\n')
r('        c.unita = r.su;\n','        c.unita = GU(215) ? -1 : r.su;\n')
s='''#include <cstdlib>
static bool GU(int n) { static const int g = std::getenv("M688_GUASTO") ? std::atoi(std::getenv("M688_GUASTO")) : 0; return g == n; }
'''+s
open('/home/randolph/Claude/melchior-688/tools/re/prove/quadro_radio/guasti_p3/missione_forze.cpp','w',encoding='utf-8').write(s)
