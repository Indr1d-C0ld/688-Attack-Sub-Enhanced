#!/bin/bash
# Le prove nuove devono diventare rosse se la cascata torna al livello A: due guasti su copie private, i sorgenti non si toccano.
# g1: la stazione non passa i campi (app/sonar_enhanced.cpp). g2: il nucleo li ignora (nucleo/src/analisi_acustica.cpp).
R=/home/randolph/Claude/melchior-688; E=$R/engine; B=$E/build_portata; C=$B/cascata; O=$R/tools/re/prove/portata_sonar/punto1/cascata_raggi/esiti/verifica
export PATH=$R/tools/venv/bin:$PATH TMPDIR=$B/tmp LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
mkdir -p "$O"; cd "$B" || exit 1
sed 's/&grana_, &scena.campi));/\&grana_));/' "$E/app/sonar_enhanced.cpp" > "$C/g1.cpp"; ! cmp -s "$E/app/sonar_enhanced.cpp" "$C/g1.cpp" || exit 1
sed 's/const CampoPerdita\* c = campi \&\& i < campi->size() ? (\*campi)\[i\] : nullptr;/const CampoPerdita* c = nullptr;/' "$E/nucleo/src/analisi_acustica.cpp" > "$C/g2.cpp"; ! cmp -s "$E/nucleo/src/analisi_acustica.cpp" "$C/g2.cpp" || exit 1
comando() { python3 - "$1" "$2" <<'P'
import json, sys
for c in json.load(open("compile_commands.json")):
    if c["file"].endswith(sys.argv[1]) and sys.argv[2] in c.get("output", c["command"]):
        print(c["command"]); break
P
}
compila() { local cmd; cmd=$(comando "$1" "$2"); cmd=$(sed -E "s# -o [^ ]+# -o $C/$3.o#; s# -c [^ ]+# -c $C/$3.cpp#; s# -MF [^ ]+# -MF $C/$3.o.d#; s# -MT [^ ]+# -MT $C/$3.o#" <<< "$cmd"); nice -n 10 bash -c "$cmd -I$E/$(dirname "$1")" 2>&1 | grep -E " error |Error " ; }
compila app/sonar_enhanced.cpp prove.dir g1 &
compila nucleo/src/analisi_acustica.cpp nucleo g2 &
wait
LP=$(ninja -t commands prove | tail -1)
L1=$(sed -E "s# -o prove # -o cascata/prove_g1 cascata/g1.o #; s#CMakeFiles/prove.dir/app/sonar_enhanced.cpp.o##" <<< "$LP")
L2=$(sed -E "s# -o prove # -o cascata/prove_g2 cascata/g2.o #" <<< "$LP")
nice -n 10 bash -c "$L1" || exit 1
nice -n 10 bash -c "$L2" || exit 1
cd "$E" || exit 1
{
for g in cascata/prove_cascata cascata/prove_g1 cascata/prove_g2; do
    echo "=== $g"; nice -n 10 "$B/$g" -tc='*perdita del sensore*,*regola del sensore*' 2>&1 | grep -E "ERROR|test cases|assertions|Status" | cut -c1-200 | sort | uniq -c | sort -rn | head -12
done
} | tee "$O/guasto.txt"
rm -f "$C/prove_g1" "$C/prove_g2" "$C/g1.o" "$C/g2.o"
