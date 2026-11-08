#!/bin/bash
# come compila_v.sh, ma rifa' solo la copia di sonar_enhanced
R=/home/randolph/Claude/melchior-688; E=$R/engine; B=$E/build_portata; C=$B/cascata
export PATH=$R/tools/venv/bin:$PATH TMPDIR=$B/tmp
python3 "$R/tools/re/prove/portata_sonar/punto1/cascata_raggi/variante.py" "$C" || exit 1
cd "$B" || exit 1
cmd=$(python3 - <<'P'
import json
for c in json.load(open("compile_commands.json")):
    if c["file"].endswith("app/sonar_enhanced.cpp") and "prove.dir" not in c["command"]:
        print(c["command"]); break
P
)
cmd=$(sed -E "s# -o [^ ]+# -o $C/sonar_enhanced_v.o#; s# -c [^ ]+# -c $C/sonar_enhanced_v.cpp#; s# -MF [^ ]+# -MF $C/sonar_enhanced_v.o.d#; s# -MT [^ ]+# -MT $C/sonar_enhanced_v.o#" <<< "$cmd")
nice -n 10 bash -c "$cmd -I$E/app" 2>&1 | grep -E "error|Error" 
L=$(ninja -t commands melchior688 | tail -1)
L=$(sed -E "s# -o melchior688 # -o cascata/melchior688_v cascata/sonar_enhanced_v.o cascata/missione_v.o #; s#CMakeFiles/melchior688.dir/app/sonar_enhanced.cpp.o##" <<< "$L")
nice -n 10 bash -c "$L" || exit 1
ls -la "$C/melchior688_v"
