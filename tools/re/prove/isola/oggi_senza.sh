#!/bin/bash
# oggi_senza.sh: i sorgenti comuni di oggi SENZA il passo 1, in albero_prima e b_prima (nessuna copia nuova): per vedere se le prove rosse dopo la portata dipendono dal passo 1
E=/home/randolph/Claude/melchior-688/engine; B=$E/build_danni; A=$B/albero_prima/engine; K=$B/mie; rm -rf $K; mkdir -p $K
for f in app/interfaccia.cpp app/interfaccia.hpp nucleo/include/nucleo/battello.hpp nucleo/include/nucleo/ordini.hpp nucleo/src/battello.cpp nucleo/src/ordini.cpp missioni/src/danni_sottomarini.cpp missioni/src/registro_missione.cpp test/prove_battello.cpp test/prove_danni_sottomarini.cpp; do mkdir -p $K/$(dirname $f); cp -p $A/$f $K/$f; done
rsync -a --delete --exclude '/build*' $E/ $A/ || exit 1
cp -rp $K/. $A/; rm -rf $K
python3 -I - "$A" <<'PY'
import sys
A=sys.argv[1]
f=A+'/missioni/src/missione.cpp'; s=open(f,encoding='utf-8').read()
a="""        case TipoOrdine::isola_compartimento:   // il controllo danni: DanniSottomarini::consegna
        case TipoOrdine::priorita_sistema:
"""
assert s.count(a)==1; open(f,'w',encoding='utf-8').write(s.replace(a,""))
f=A+'/CMakeLists.txt'; s=open(f,encoding='utf-8').read()
a="test/prove_pagina_squadre.cpp"
print('righe della prova del kit:',s.count(a))
s=s.replace("  "+a+"\n","").replace(" "+a,"")
open(f,'w',encoding='utf-8').write(s)
PY
export PATH=/home/randolph/Claude/melchior-688/tools/venv/lib/python3.13/site-packages/cmake/data/bin:$PATH TMPDIR=$B/tmp
nice -n 19 cmake --build $B/b_prima -j6 --target prove 2>&1 | grep -E "error|FAILED|ninja: |Linking CXX executable" | cut -c1-200 | tail -5
cp $B/b_prima/prove $B/oggi/prove_senza || exit 1
cd $B/oggi
for p in "*asw missione: sul battello in superficie*" "*quadro ai sottomarini: col contatto ostile*" "*quadro ai sottomarini: il contatto proprio*"; do echo -n "senza passo 1, $p: "; nice -n 19 ./prove_senza -tc="$p" 2>&1 | grep -E "test cases" | cut -c1-70; done
