#!/bin/bash
# melchior688_v: il binario delle immagini, build_portata con le due copie private (variante.py). Solo a build ferma.
R=/home/randolph/Claude/melchior-688; E=$R/engine; B=$E/build_portata; C=$B/cascata
export PATH=$R/tools/venv/bin:$PATH TMPDIR=$B/tmp
python3 "$R/tools/re/prove/portata_sonar/punto1/cascata_raggi/variante.py" "$C" || exit 1
cd "$B" || exit 1
comando() { python3 - "$1" <<'P'
import json, sys
for c in json.load(open("compile_commands.json")):
    if c["file"].endswith(sys.argv[1]) and "prove.dir" not in c.get("output", "") and "prove.dir" not in c["command"]:
        print(c["command"]); break
P
}
compila() {  # sorgente vero, copia, oggetto
    local cmd; cmd=$(comando "$1")
    cmd=$(sed -E "s# -o [^ ]+# -o $3#; s# -c [^ ]+# -c $2#; s# -MF [^ ]+# -MF $3.d#; s# -MT [^ ]+# -MT $3#" <<< "$cmd")
    nice -n 10 bash -c "$cmd -I$E/$(dirname "$1")" || exit 1
}
compila app/sonar_enhanced.cpp "$C/sonar_enhanced_v.cpp" "$C/sonar_enhanced_v.o" &
compila missioni/src/missione.cpp "$C/missione_v.cpp" "$C/missione_v.o" &
wait
L=$(ninja -t commands melchior688 | tail -1)
L=$(sed -E "s# -o melchior688 # -o cascata/melchior688_v cascata/sonar_enhanced_v.o cascata/missione_v.o #; s#CMakeFiles/melchior688.dir/app/sonar_enhanced.cpp.o##" <<< "$L")
nice -n 10 bash -c "$L" || exit 1
ls -la "$C/melchior688_v"
