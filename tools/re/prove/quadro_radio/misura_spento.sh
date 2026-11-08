#!/bin/bash
# le stesse partite delle missioni vere col quadro spento (binario con l'interruttore forzato), per il confronto
R=/home/randolph/Claude/melchior-688; S=$R/engine/build_radio/fisso/melchior688_spento; U=$R/engine/build_radio/misura_spento
mkdir -p "$U/tmp"; export TMPDIR=$U/tmp LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
cd "$R/engine" || exit 1
grep -v '^m4\|^s5' "$R/engine/build_radio/misura/lavori.txt" > "$U/lavori.txt"
gioca() { n=$1; shift; nice -n 15 "$S" --sandbox "$@" --salva-traccia "$U/$n.m688tr" --istantanea "$U/$n.bmp" > "$U/$n.txt" 2>&1; rm -f "$U/$n.bmp"; echo "$n rc $?" >> "$U/fatti.txt"; }
export -f gioca; export S U
xargs -P 7 -L 1 bash -c 'gioca "$@"' _ < "$U/lavori.txt"
date > "$U/fine.txt"
