#!/bin/bash
# ricompila le prove nel binario di build_radio, poi lancia la prova a 200 semi e tutte quelle dei rapporti
R=/home/randolph/Claude/melchior-688; B=$R/engine/build_radio; F=$B/fisso_p3; P=$F/esiti
cd "$B" || exit 1
TMPDIR=$B/tmp nice -n 10 "$R/tools/venv/bin/ninja" -j5 prove 2>&1 | tail -1
cp prove "$F/prove_nuove"
export TMPDIR=$F/tmp LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
cd "$R/engine" || exit 1
nice -n 10 "$F/prove_nuove" -ns -s -tc='rapporti dei sottomarini: 200*' > "$P/rapporti_200.txt" 2>&1; echo "200 semi rc $?"; grep -E 'su 200|ERROR|Status' "$P/rapporti_200.txt"
nice -n 10 "$F/prove_nuove" -tc='rapporti dei sottomarini*' > "$P/rapporti.txt" 2>&1; echo "rapporti rc $?"; tail -2 "$P/rapporti.txt"
date
