#!/usr/bin/env python3
# La copia del tiro con i contatori, per sonda_perche.cpp: non tocca i sorgenti. uso: copia_tiro.py SORGENTE COPIA [REAZIONI COPIA_REAZIONI]
import sys
t = open(sys.argv[1]).read()
def sost(a, b):
    global t
    assert t.count(a) == 1, a
    t = t.replace(a, b)
sost('namespace m688 {\n\nnamespace {\n', '#include <cstdlib>\n#include <map>\nextern std::map<int, std::string> g_perche;\nextern std::map<int, int> g_amico;\nextern std::map<int, double> g_laterale, g_oltre, g_gittata;\nnamespace m688 {\n\nnamespace {\n')
sost('            auto& n = it->second;\n            // in pace, solo', '            auto& n = it->second;\n            g_perche[id] = "guerra_o_pezzi";\n            // in pace, solo')
sost('            const bool visto = scelto.has_value();\n', '            const bool visto = scelto.has_value();\n            g_perche[id] = visto ? "visto_poi" : n.acquisito ? "sparito_entro_cessate" : "non_visto";\n')
sost('                } else {\n                    continue;\n                }\n            } else {\n                continue;\n            }\n',
     '                } else {\n                    g_perche[id] = "sparito_oltre_cessate";\n                    continue;\n                }\n            } else {\n                continue;\n            }\n')
sost("if (std::fabs(normalizza_180(rilevamento(u.pos, f.pos) - rlv)) < FUOCO_AMICO_GRADI) { amico = true; break; }",
     "if (std::fabs(normalizza_180(rilevamento(u.pos, f.pos) - rlv)) < FUOCO_AMICO_GRADI) { amico = true; g_amico[id] = fid; g_laterale[id] = distanza(u.pos, f.pos) * std::fabs(std::sin(radianti(rilevamento(u.pos, f.pos) - rlv))); g_oltre[id] = distanza(u.pos, f.pos) - d; g_gittata[id] = d; break; }")
sost('            if (amico) continue;\n', '            if (std::getenv("SONDA_SENZA_AMICO")) amico = false;\n            if (amico) { g_perche[id] = "amico_" + std::to_string(g_amico[id]); continue; }\n            g_perche[id] = "tiro";\n')
sost('                if (tempo_ < p.prossimo || tempo_ < p.pausa_fino) continue;\n',
     '                if (tempo_ < p.pausa_fino) { g_perche[id] = "ricarica"; continue; }\n                if (tempo_ < p.prossimo) { g_perche[id] = "cadenza_o_reazione"; continue; }\n')
sost('                if (p.dati->deposito > 0 && p.consumati >= deposito) continue;\n', '                if (p.dati->deposito > 0 && p.consumati >= deposito) { g_perche[id] = "deposito"; continue; }\n')
sost('                if (portata <= 0.0 || d > portata || d < p.dati->portata_minima_m) continue;\n', '                if (portata <= 0.0 || d > portata || d < p.dati->portata_minima_m) { g_perche[id] = "portata"; continue; }\n                g_perche[id] = "spara";\n')
open(sys.argv[2], 'w').write(t)
# la controprova del punto 10a: con SONDA_TUTTE sul contatto scoperto va ogni nave col sonar, non solo la piu' vicina
if len(sys.argv) > 4:
    t = open(sys.argv[3]).read()
    sost('            if (k == asw) {\n', '            if (k == asw || (std::getenv("SONDA_TUTTE") && v.categoria == "nave" && evento == "scoperto")) {\n')
    t = '#include <cstdlib>\n' + t
    open(sys.argv[4], 'w').write(t)
