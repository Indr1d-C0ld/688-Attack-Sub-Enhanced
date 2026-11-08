#!/bin/bash
# costruisce prove_guasti: gli oggetti guastati davanti a libmissioni.a, nel resto il binario di build_radio
set -e
R=/home/randolph/Claude/melchior-688
B=$R/engine/build_radio
G=$R/tools/re/prove/quadro_radio/guasti
cd $G && python3 applica.py
nice -n 10 $R/tools/venv/bin/ninja -C $B prove > $B/costruzione.log 2>&1 || { tail -30 $B/costruzione.log | cut -c1-300; exit 1; }
cd $B
for f in missione_forze missione_interventi; do
  python3 - "$f" "$G" > $G/cc_$f.sh <<'PY'
import json,sys,shlex
f,g=sys.argv[1:]
for e in json.load(open('compile_commands.json')):
    if e['file'].endswith('/missioni/src/%s.cpp'%f):
        a=shlex.split(e['command'])
        i=a.index('-o'); a[i+1]='%s/%s.o'%(g,f)
        a=[x for x in a if not x.endswith('/missioni/src/%s.cpp'%f)]
        a=[x for x in a if x not in ('-MD',)]
        if '-MF' in a:
            k=a.index('-MF'); del a[k:k+2]
        if '-MT' in a:
            k=a.index('-MT'); del a[k:k+2]
        if '-c' in a: a.remove('-c')
        print(' '.join(shlex.quote(x) for x in a)+' -c %s/%s.cpp'%(g,f))
PY
  nice -n 10 bash $G/cc_$f.sh &
done
wait
L=$($R/tools/venv/bin/ninja -t commands prove | tail -1)
L=$(echo "$L" | sed -e "s#-o prove #-o prove_guasti #" -e "s#libmissioni.a#$G/missione_forze.o $G/missione_interventi.o libmissioni.a#")
nice -n 10 bash -c "$L"
ls -la prove_guasti
