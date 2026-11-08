#!/bin/bash
# costruisce prove_guasti_p3: l'oggetto guastato davanti a libmissioni.a, nel resto il binario di build_radio. A build ferma.
set -e
R=/home/randolph/Claude/melchior-688; B=$R/engine/build_radio; G=$R/tools/re/prove/quadro_radio/guasti_p3
export TMPDIR=$B/tmp_g; mkdir -p "$TMPDIR"
python3 "$G/applica.py"
cd "$B"
python3 - "$G" > "$G/cc.sh" <<'PY'
import json,sys,shlex
g=sys.argv[1]
for e in json.load(open('compile_commands.json')):
    if e['file'].endswith('/missioni/src/missione_forze.cpp'):
        a=shlex.split(e['command'])
        a[a.index('-o')+1]=g+'/missione_forze.o'
        a=[x for x in a if not x.endswith('/missioni/src/missione_forze.cpp') and x not in ('-MD','-c')]
        for o in ('-MF','-MT'):
            if o in a:
                k=a.index(o); del a[k:k+2]
        print(' '.join(shlex.quote(x) for x in a)+' -c %s/missione_forze.cpp'%g)
PY
nice -n 10 bash "$G/cc.sh"
L=$("$R/tools/venv/bin/ninja" -t commands prove | tail -1)
L=$(echo "$L" | sed -e "s#-o prove #-o prove_guasti_p3 #" -e "s#libmissioni.a#$G/missione_forze.o libmissioni.a#")
nice -n 10 bash -c "$L"
ls -la prove_guasti_p3
