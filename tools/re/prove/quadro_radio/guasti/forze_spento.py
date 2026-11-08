# una copia di missione_forze.cpp col quadro sempre spento: il motore per il confronto con la serie dopo delle difese
s=open('/home/randolph/Claude/melchior-688/engine/missioni/src/missione_forze.cpp',encoding='utf-8').read()
a='forze_->quadro_sottomarini = !quadro_sottomarini_spento_ && armi_ia_ && !armi_ia_->originale;'
assert s.count(a)==1
open('/home/randolph/Claude/melchior-688/tools/re/prove/quadro_radio/guasti/forze_spento.cpp','w',encoding='utf-8').write(s.replace(a,'forze_->quadro_sottomarini = false;'))
