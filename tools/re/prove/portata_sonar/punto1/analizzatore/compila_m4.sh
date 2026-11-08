#!/bin/bash
# melchior688_m4: il gioco di oggi, compilato com'e', con la sola sonda delle campagne (spazza_gioco di m3) innestata. Il passo della colonna e' passo_da_voci vera,
# piu' una colonna con la scritta dello schermo. Si legano gli oggetti di build_portata: niente analisi_v.o.
R=/home/randolph/Claude/melchior-688; E=$R/engine; B=$E/build_portata; C=$B/analizzatore
export PATH=$R/tools/venv/bin:$PATH TMPDIR=$B/tmp
python3 - "$E/app/sonar_enhanced.cpp" "$C/sonar_enhanced_m3.cpp" "$C/sonar_enhanced_m4.cpp" <<'P'
import sys
sor,m3,dest=sys.argv[1:4]; t=open(sor).read(); L=open(m3).read().split("\n")
testa="\n".join(L[:33])+"\n"   # gli include e il define private public
assert L[32]=="#define private public" and L[34]=="#undef private", (L[32],L[34])
a=t.index("\n")+1
t=testa+t[:a]+"#undef private\n#include <cstdlib>\n#include <cstdio>\n#include <map>\n#include <sstream>\nnamespace m688 { static const std::map<int, const CampoPerdita*>* g_campi_lofar = nullptr; }\n"+t[a:]
sonda="\n".join(L[270:516])+"\n"
assert sonda.count("passo_da_ipotesi(ipotesi)")==1
sonda=sonda.replace("passo_da_ipotesi(ipotesi)","passo_da_voci(confronti, cat)")
x='''std::fprintf(f, "%s\\t%d\\t%s\\t%s\\t%s\\t%.1f\\t%.0f\\n", voci(confronti).c_str(), tolte, voci(confronta_rapporti(senza_lati, cat, pale_l)).c_str(), g->classe.c_str(), s.sensore_.c_str(), sc.ricevitore.velocita_kn, sc.ricevitore.cavo_filato_m);'''
assert sonda.count(x)==1, "fprintf"
sonda=sonda.replace(x,'''std::fprintf(f, "%s\\t%d\\t%s\\t%s\\t%s\\t%.1f\\t%.0f\\t%s\\n", voci(confronti).c_str(), tolte, voci(confronta_rapporti(senza_lati, cat, pale_l)).c_str(), g->classe.c_str(), s.sensore_.c_str(), sc.ricevitore.velocita_kn, sc.ricevitore.cavo_filato_m, s.lettura_analisi(14).testo.c_str());''')
x="void SonarEnhanced::aggiorna(const Missione& m, int bersaglio) {\n"
assert t.count(x)==1
t=t.replace(x,sonda+x+"    static bool dentro = false;\n    if (!dentro) { dentro = true; spazza_gioco(*this, m); dentro = false; }\n")
open(dest,"w").write(t)
P
cd "$B" || exit 1
K=$(ninja -t commands melchior688 | grep -F "CMakeFiles/melchior688.dir/app/sonar_enhanced.cpp.o" | head -1)
K=$(sed -E "s#-o CMakeFiles/melchior688.dir/app/sonar_enhanced.cpp.o#-o $C/sonar_enhanced_m4.o -I$E/app#; s#-c [^ ]*app/sonar_enhanced.cpp#-c $C/sonar_enhanced_m4.cpp#; s# -g # #; s#-MF [^ ]*#-MF $C/m4.d#; s#-MT [^ ]*#-MT m4.o#" <<< "$K")
nice -n 10 bash -c "$K" 2>&1 | grep -E "error" | cut -c1-300
[ -f "$C/sonar_enhanced_m4.o" ] || exit 1
Q=$(ninja -t commands melchior688 | tail -1 | sed -E 's/^: && //; s/ && :$//')
Q=$(sed -E "s# -o melchior688 # -o analizzatore/melchior688_m4 analizzatore/sonar_enhanced_m4.o #; s#CMakeFiles/melchior688.dir/app/sonar_enhanced.cpp.o##" <<< "$Q")
nice -n 10 bash -c "$Q" || exit 1
strip --strip-debug "$C/melchior688_m4"; ls -la "$C/melchior688_m4"; rm -f "$C/sonar_enhanced_m4.o" "$C/m4.d"
