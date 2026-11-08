#!/bin/bash
# Le prove di P3 sulla copia dei binari: quadro ai sottomarini, i rapporti a 200 semi (a mano), il resto di prove_forze.
R=/home/randolph/Claude/melchior-688
F=$R/engine/build_radio/fisso_p3
P=$F/esiti
mkdir -p "$P" "$F/tmp"
export TMPDIR=$F/tmp LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
cd "$R/engine" || exit 1
nice -n 10 "$F/prove" -ns -tc='rapporti dei sottomarini*' > "$P/rapporti.txt" 2>&1; echo "rapporti rc $?"; tail -3 "$P/rapporti.txt"
nice -n 10 "$F/prove" -tc='quadro ai sottomarini*' > "$P/quadro.txt" 2>&1; echo "quadro rc $?"; tail -3 "$P/quadro.txt"
date
