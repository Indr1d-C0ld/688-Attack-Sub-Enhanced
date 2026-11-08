#!/bin/bash
# Il binario col quadro radio forzato a spento (lo stesso guasto di quadro_radio/guasti/forze_spento.py), dai sorgenti di adesso e con gli
# oggetti di build_difese. uso: spento.sh USCITA
set -e
R=/home/randolph/Claude/melchior-688; B=$R/engine/build_difese; G=$B/tmp/spento; U=$1
mkdir -p "$G"; export PATH=$R/tools/venv/bin:$PATH TMPDIR=$B/tmp
"$R/tools/venv/bin/python" -I - "$G" "$R" <<'PY'
import json, shlex, sys
g, r = sys.argv[1:3]
s = open(r + '/engine/missioni/src/missione_forze.cpp', encoding='utf-8').read()
a = 'forze_->quadro_sottomarini = !quadro_sottomarini_spento_ && armi_ia_ && !armi_ia_->originale;'
assert s.count(a) == 1
open(g + '/forze_spento.cpp', 'w', encoding='utf-8').write(s.replace(a, 'forze_->quadro_sottomarini = false;'))
for e in json.load(open(r + '/engine/build_difese/compile_commands.json')):
    if e['file'].endswith('/missioni/src/missione_forze.cpp'):
        c = shlex.split(e['command'])
        c[c.index('-o') + 1] = g + '/forze_spento.o'
        c = [x for x in c if not x.endswith('/missioni/src/missione_forze.cpp') and x not in ('-MD', '-c')]
        for k in ('-MF', '-MT'):
            if k in c:
                j = c.index(k); del c[j:j + 2]
        open(g + '/cc.sh', 'w').write(' '.join(shlex.quote(x) for x in c) + ' -c ' + g + '/forze_spento.cpp\n')
PY
cd "$B"
nice -n 19 bash "$G/cc.sh" 2>&1 | grep -E " error " || true
L=$(ninja -t commands melchior688 | tail -1)
L=$(echo "$L" | sed -e "s#-o melchior688 #-o $U #" -e "s#libmissioni.a#$G/forze_spento.o libmissioni.a#")
nice -n 19 bash -c "$L"
md5sum "$U" | cut -c1-12
