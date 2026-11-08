#!/bin/bash
set -e
R=/home/randolph/Claude/melchior-688; B=$R/engine/build_radio; G=$R/tools/re/prove/quadro_radio/guasti
python3 $G/forze_spento.py
cd $B
python3 - "$G" > $G/cc_spento.sh <<'PY'
import json,sys,shlex
g=sys.argv[1]
for e in json.load(open('compile_commands.json')):
    if e['file'].endswith('/missioni/src/missione_forze.cpp'):
        a=shlex.split(e['command'])
        i=a.index('-o'); a[i+1]=g+'/forze_spento.o'
        a=[x for x in a if not x.endswith('/missioni/src/missione_forze.cpp') and x!='-MD' and x!='-c']
        for k in ('-MF','-MT'):
            if k in a:
                j=a.index(k); del a[j:j+2]
        print(' '.join(shlex.quote(x) for x in a)+' -c '+g+'/forze_spento.cpp')
PY
nice -n 10 bash $G/cc_spento.sh 2>&1 | grep -E " error " || true
L=$($R/tools/venv/bin/ninja -t commands melchior688 | tail -1)
L=$(echo "$L" | sed -e "s#-o melchior688 #-o fisso/melchior688_spento #" -e "s#libmissioni.a#$G/forze_spento.o libmissioni.a#")
nice -n 10 bash -c "$L"
