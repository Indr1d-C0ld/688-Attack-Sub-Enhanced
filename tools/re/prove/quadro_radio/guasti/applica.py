# Copie guastate di missione_forze.cpp e missione_interventi.cpp: ogni guasto si accende con M688_GUASTO=<n> (albero condiviso intatto)
R='/home/randolph/Claude/melchior-688/engine/missioni/src/'
G='''#include <cstdlib>
static bool GU(int n) { static const int g = std::getenv("M688_GUASTO") ? std::atoi(std::getenv("M688_GUASTO")) : 0; return g == n; }
'''
s=open(R+'missione_forze.cpp',encoding='utf-8').read()
def r(a,b,quante=1):
    global s
    assert s.count(a)==quante,(a,s.count(a))
    s=s.replace(a,b)
r('if (r == F.radio_sottomarini.end() || r->second.esposto_da < 0.0 || tempo_ - r->second.esposto_da < COPIA_S) return false;',
  'if (!GU(1) && (r == F.radio_sottomarini.end() || r->second.esposto_da < 0.0 || tempo_ - r->second.esposto_da < (GU(3) ? 0.0 : COPIA_S))) return false;')
r('return std::fabs(v.velocita) <= RADIO.velocita_albero_kn || F.caso_radio.uniforme() < 0.5;',
  'if (GU(41) && std::fabs(v.velocita) > RADIO.velocita_albero_kn) return false; return GU(4) || std::fabs(v.velocita) <= RADIO.velocita_albero_kn || F.caso_radio.uniforme() < 0.5;')
r('const bool esposto = a_quota_antenna(u) && !sotto_banchisa(id);',
  'const bool esposto = GU(2) ? ((u.flag54 & 6) != 0 || u.quota <= 20.0) : a_quota_antenna(u) && (GU(52) || !sotto_banchisa(id)); if ((GU(51) || GU(52)) && std::fmod(tempo_, 300.0) < 1.0) fprintf(stderr, "D t=%.0f id=%d q=%.1f esp=%d esposto_da=%.0f ciclo=%.0f sit=%d\\n", tempo_, id, u.quota, esposto, R.esposto_da, F.ciclo_s[u.flag & 0x18], static_cast<int>(situazione_di(u.flag & 0x18, 0x08)));')
r('&& !contatto_ostile(RIMANDA_M[livello]) && !sotto_banchisa(id)) {',
  '&& (GU(83) || !contatto_ostile(GU(84) ? QUALUNQUE_M : RIMANDA_M[livello])) && (GU(51) || !sotto_banchisa(id))) {')
r('if (script_ora || occupato || contatto_ostile(RIMANDA_M[livello]) ||',
  'if (script_ora || occupato || (!GU(83) && contatto_ostile(GU(84) ? QUALUNQUE_M : RIMANDA_M[livello])) ||')
r('''        const auto contatto_ostile = [&, &u = u](double entro_m) {''','''        if (GU(51) && std::fmod(tempo_, 100.0) < 1.0) fprintf(stderr, "O t=%.0f aff=%d dif=%d rea=%d tma=%d agg=%d ml=%d ped=%d ov=%d ev=%d nav=%d\\n", tempo_, u.affondamento, (int)manovre_difesa_.count(id), (int)(rz != reazioni_in_corso_.end() && tempo_ < rz->second.fine), (int)(tm != armi_ia_->tma.end() && tm->first.first == id), u.agganciato, (int)(ml != missili_ia_.end() && ml->second.in_manovra), (int)pedine_vicine_.count(id), (int)ordine_vetrino, (int)guida_evasione(u).has_value(), (int)u.navigazione);
        const auto contatto_ostile = [&, &u = u](double entro_m) {''')
r('if (dz.perdita > 0.0 && F.caso_radio.uniforme() < dz.perdita) continue;','if (!GU(61) && dz.perdita > 0.0 && F.caso_radio.uniforme() < dz.perdita) continue;')
r('const double arrivo = tempo_ + ritardo(flash ? ch.flash_s : ch.immediate_s) * dz.ritardo;','const double arrivo = tempo_ + ritardo(flash ? ch.flash_s : ch.immediate_s) * (GU(62) ? 1.0 : dz.ritardo);')
r('''bool perso = F.caso.uniforme() < F.d.teatro_perdita;
                if (!perso && disturbato()) {''','''bool perso = F.caso.uniforme() < F.d.teatro_perdita;
                if (!perso && disturbato() && !GU(63)) {''')
r('F.quadro_sottomarini && u.categoria == "sottomarino" && ciclo > 0.0','!GU(71) && F.quadro_sottomarini && (GU(72) || u.categoria == "sottomarino") && ciclo > 0.0')
r('prossimo != R.ciclo_tentato && ','(GU(87) || prossimo != R.ciclo_tentato) && ')
r('situazione_di(lu, altra) != Situazione::pace &&','(GU(81) || situazione_di(lu, altra) != Situazione::pace) &&')
r('''prossimo >= R.ultimo_ascolto + (a >= 3 ? ASCOLTO_A3_OGNI_S : ASCOLTO_OGNI_S) &&''','''(GU(85) || prossimo >= R.ultimo_ascolto + (a >= 3 && !GU(82) ? ASCOLTO_A3_OGNI_S : ASCOLTO_OGNI_S)) &&''')
r('return armi_libere_ia(id, rb).value_or(false);','return GU(86) || armi_libere_ia(id, rb).value_or(false);')
r('if (R.caccia < 0 && !occupato && !contatto_suo && !R.salita && !script_ora && tempo_ >= R.caccia_non_prima) {','if (!GU(91) && R.caccia < 0 && !occupato && !contatto_suo && !R.salita && !script_ora && tempo_ >= R.caccia_non_prima) {')
r('} else if (occupato || contatto_suo) {','} else if (!GU(92) && (occupato || contatto_suo)) {')
r('cr.raggio_m > LIMITE_ELLISSE_M','(!GU(93) && cr.raggio_m > LIMITE_ELLISSE_M)')
r('(a >= 3 && !cr.posizione_valida)','(!GU(97) && a >= 3 && !cr.posizione_valida)')
r('velocita_di_prima(R.velocita_prima_caccia);','if (!GU(94)) velocita_di_prima(R.velocita_prima_caccia);')
r('if (const auto l = R.lasciati.find(rb); l != R.lasciati.end() && cr.r.t_fix <= l->second) return false;','if (const auto l = R.lasciati.find(rb); !GU(95) && l != R.lasciati.end() && cr.r.t_fix <= l->second) return false;')
r('''    if (F.quadro_sottomarini)
        for (auto& [id, R] : F.radio_sottomarini) {''','''    if (F.quadro_sottomarini && !GU(96))
        for (auto& [id, R] : F.radio_sottomarini) {''')
r('if (rotta && !u.navigazione) u.rotta_ordinata = R.rotta_prima_caccia;','if (!GU(98) && rotta && !u.navigazione) u.rotta_ordinata = R.rotta_prima_caccia;')
r('R.velocita_caccia = std::min(KN_CACCIA[livello], u.avanti_tutta);','''R.velocita_caccia = GU(99) ? 20.0 : std::min(KN_CACCIA[livello], u.avanti_tutta);
                if (GU(10) && !u.g_bersaglio.empty()) {   // il dato radio fa da contatto: il gestore lancia
                    auto& c = u.contatti[R.caccia];
                    c.qualita = 1000.0; c.posizione_nota = true; c.pos_stimata = R.verso; c.t_posizione = tempo_;
                    u.bersaglio = R.caccia;
                    const std::string gestore = u.g_bersaglio;
                    u.g_bersaglio.clear();
                    esegui(id, gestore);
                    fprintf(stderr, "G10 t=%.0f gestore=%s contatti=%zu unita=%zu errori=%zu %s\\n", tempo_, gestore.c_str(), u.contatti.size(), unita_.size(), errori_.size(), errori_.empty() ? "" : errori_.back().c_str());
                }''')
r('if (cr.r.categoria == "aereo" || (b->flag & 3) == 3 || b->categoria == "aereo") return false;','if (!GU(11) && (cr.r.categoria == "aereo" || (b->flag & 3) == 3 || b->categoria == "aereo")) return false;')
r('forze_->quadro_sottomarini = !quadro_sottomarini_spento_ && armi_ia_ && !armi_ia_->originale;','forze_->quadro_sottomarini = (GU(122) || !quadro_sottomarini_spento_) && armi_ia_ && (GU(121) || !armi_ia_->originale);')
r('sub && (F.quadro_sottomarini ? a_quota_antenna(u) && !sotto_banchisa(id) :','sub && (F.quadro_sottomarini ? a_quota_antenna(u) && (GU(54) || !sotto_banchisa(id)) :')
i=s.index('extern "C"')
s=s[:i]+G+s[i:]
open('missione_forze.cpp','w',encoding='utf-8').write(s)
s=open(R+'missione_interventi.cpp',encoding='utf-8').read()
r('a_quota_antenna(u) && !sotto_banchisa(id)','a_quota_antenna(u) && (GU(53) || !sotto_banchisa(id))')
i=s.index('namespace m688')
s=s[:i]+G+s[i:]
open('missione_interventi.cpp','w',encoding='utf-8').write(s)
