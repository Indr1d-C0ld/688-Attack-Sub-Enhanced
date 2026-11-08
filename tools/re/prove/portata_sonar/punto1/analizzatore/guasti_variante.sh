#!/bin/bash
# La variante del POSSUB col suo guasto, e di nuovo quelli di P1 e P2 adattati: copia privata di app/sonar_enhanced.cpp, le prove devono diventare rosse. Sorgenti non toccati.
R=/home/randolph/Claude/melchior-688; E=$R/engine; B=$E/build_portata; C=$B/analizzatore; G=$C/guasti
export PATH=$R/tools/venv/bin:$PATH TMPDIR=$B/tmp LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
mkdir -p "$G"; cd "$B" || exit 1
K=$(ninja -t commands prove | grep -F "CMakeFiles/prove.dir/app/sonar_enhanced.cpp.o" | head -1)
Q=$(ninja -t commands prove | tail -1 | sed -E 's/^: && //; s/ && :$//')
guasto() { # nome vecchio nuovo
    python3 - "$E/app/sonar_enhanced.cpp" "$G/$1.cpp" "$2" "$3" <<'P'
import sys
s,d,a,b=sys.argv[1:5]; t=open(s).read()
assert t.count(a)==1, a
open(d,"w").write(t.replace(a,b))
P
    local kk; kk=$(sed -E "s#-o CMakeFiles/prove.dir/app/sonar_enhanced.cpp.o#-o $G/$1.o#; s#-c [^ ]*app/sonar_enhanced.cpp#-c $G/$1.cpp#; s# -g # #; s#-MF [^ ]*#-MF $G/$1.d#; s#-MT [^ ]*#-MT $1.o#" <<< "$K")
    nice -n 10 bash -c "$kk" 2>&1 | grep -E "error" | cut -c1-200
    local qq; qq=$(sed -E "s#CMakeFiles/prove.dir/app/sonar_enhanced.cpp.o#$G/$1.o#; s# -o prove # -o $G/$1 #" <<< "$Q")
    nice -n 10 bash -c "$qq" || { echo "link $1 fallito"; return; }
    echo "== guasto $1"
    (cd "$E" && "$G/$1" -tc="sonar enhanced*" 2>&1 | grep -E "^TEST CASE|Status|test cases:" | cut -c1-120 | sort -u)
    rm -f "$G/$1" "$G/$1.o"
}
guasto possub_fuori 'return sub == 0 ? PassoClassifica::nessuno : sub == mostrate ? PassoClassifica::probsub : PassoClassifica::possub;' 'if (sub == mostrate && sub) return PassoClassifica::probsub;
    if (rette.empty()) return PassoClassifica::nessuno;
    const int vicino = std::max(2, rette.front().spiegate - 1);
    for (const auto& c : v)
        if (voce_sottomarino(cat, c.id) && righe_spiegate(c) >= vicino) return PassoClassifica::possub;
    return PassoClassifica::nessuno;'
guasto soglie 'if (o >= RETTA_SPIEGATE && della_voce >= RETTA_DELLA_VOCE && o >= RETTA_QUOTA * lette) out.push_back({&c, o});' 'if (o >= 0 && della_voce >= 0 && lette >= 0.0) out.push_back({&c, o});'
guasto azzera '    confronti_.clear();   // senza righe si torna prima: la pagina non deve restare sulla lettura del fascio di prima
    famiglie_.clear();
' ''
guasto nonsub 'return sub == 0 ? PassoClassifica::nessuno :' 'return sub == 0 ? PassoClassifica::nonsub :'
guasto probsub 'sub == mostrate ? PassoClassifica::probsub :' 'sub == mostrate ? PassoClassifica::certsub :'
rm -rf "$G"
