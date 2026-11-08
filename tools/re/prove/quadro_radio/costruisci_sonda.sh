#!/bin/bash
# Compila le copie di sonda_quadro.py contro gli archivi di build_radio e collega melchior688_sonda. Solo a build ferma.
# Uso: costruisci_sonda.sh <cartella>, relativa a build_radio; le opzioni in piu' vanno a sonda_quadro.py
R=/home/randolph/Claude/melchior-688; B=$R/engine/build_radio; U=$1; shift
export PATH=$R/tools/venv/bin:$PATH TMPDIR=$B/tmp
cd "$B" || exit 1
mkdir -p "$U"
python3 "$R/tools/re/prove/quadro_radio/sonda_quadro.py" "$B/$U" "$@" || exit 1
comp() { # sorgente nel motore, file nella sonda
    c=$(python3 - "$1" <<'PY'
import json,sys
for x in json.load(open('compile_commands.json')):
    if x['file'].endswith(sys.argv[1]): print(x['command']); break
PY
)
    [ -n "$c" ] || { echo "manca $1"; exit 1; }
    c=$(echo "$c" | sed -E "s# -o [^ ]+# -o $U/$2.o#; s# -c [^ ]+# -c $B/$U/$2.cpp#; s# -MF [^ ]+##; s# -MT [^ ]+##; s# -MD##")
    echo "$c" | nice -n 10 bash || exit 1
}
comp missioni/src/missione_forze.cpp missione_forze_sonda & a=$!
comp missioni/src/missione.cpp missione_sonda & b=$!
wait $a || exit 1; wait $b || exit 1
l=$(ninja -t commands melchior688 | tail -1)
l=$(echo "$l" | sed -E "s# -o melchior688 # -o $U/melchior688_sonda $U/missione_forze_sonda.o $U/missione_sonda.o #")
echo "$l" | bash || exit 1
ls -la "$U/melchior688_sonda"
