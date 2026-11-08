#!/bin/bash
# Verifica del giro elicottero + punto_divergenza: la prova cade senza la correzione, poi rilievo, casi, sonda sulla libreria
R=/home/randolph/Claude/melchior-688
cd $R/engine
export PATH=$R/tools/venv/bin:$PATH TMPDIR=$PWD/build_acustica/tmp LD_LIBRARY_PATH=../tools/deps/sysroot/usr/lib/x86_64-linux-gnu SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy
V=$R/tools/re/prove/suite_acustica/sonda_voli
FILTRO="*elicottero della missione*,*entra per evento*,*base fissa*"
A=missioni/src/missione_antisom_navi.cpp; B=missioni/src/missione_asw.cpp
cp $A $V/A.buona; cp $B $V/B.buona
python3 - "$A" "$B" <<'PY'
import sys
a,b=sys.argv[1:]
s=open(a,encoding='utf-8').read()
o="        if (viva) a.c.sposta_base(n->pos);\n"; assert s.count(o)==1
s=s.replace(o,"        continue;\n"+o); open(a,'w',encoding='utf-8').write(s)
s=open(b,encoding='utf-8').read()
o="if (cf && hangar_persi_.count(cf->base_unita)) {"; assert s.count(o)==1
s=s.replace(o,"if (false && cf && hangar_persi_.count(cf->base_unita)) {"); open(b,'w',encoding='utf-8').write(s)
PY
ninja -C build_acustica prove 2>&1 | grep -E " error |FAILED" | cut -c1-200
echo "== senza la correzione"
timeout 300 build_acustica/prove -tc="$FILTRO" 2>&1 | grep -E "ERROR|test cases|assertions" | cut -c1-150
cp $V/A.buona $A; cp $V/B.buona $B; rm $V/A.buona $V/B.buona
ninja -C build_acustica prove melchior688 rilievo_missioni 2>&1 | grep -E " error |FAILED" | cut -c1-200
echo "== con la correzione"
timeout 300 build_acustica/prove -tc="$FILTRO,*hangar bruciato*,confronto:*" 2>&1 | grep -E "ERROR|test cases|assertions" | cut -c1-150
echo "== rilievo"
mkdir -p $V/casi $V/rilievo
CARTELLA=$V/rilievo ../tools/regressione/rilievo_classico.sh build_acustica/rilievo_missioni > $V/rilievo.txt 2>&1
tail -2 $V/rilievo.txt | cut -c1-140
echo "== casi"
CARTELLA=$V/casi ../tools/regressione/casi.sh build_acustica/melchior688 > $V/casi.txt 2>&1
wc -l < $V/casi.txt
diff $V/casi.txt ../tools/regressione/riferimento.txt | cut -c1-150
echo "== sonda"
cd build_acustica
/usr/bin/c++ -DM688_DATI=\"$R/data/motore\" -DM688_MISSIONI=\"$R/data/missioni\" -I$R/engine/missioni/include -I$R/engine/nucleo/include -isystem $R/engine/terzi/ymfm -O2 -std=c++20 -c $V/sonda.cpp -o $V/sonda.o && \
/usr/bin/c++ $V/sonda.o -o $V/sonda libmissioni.a libnucleo.a libymfm.a liblua54.a -lm || exit 1
cd $R/engine
grep -l "base_unita" ../data/missioni/libreria/*.lua | xargs -P 6 -I{} timeout 900 $V/sonda {} 6 > $V/sonda.txt 2>$V/sonda.err
sort $V/sonda.txt | cut -c1-170
wc -l < $V/sonda.txt
