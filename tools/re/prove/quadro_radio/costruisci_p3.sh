#!/bin/bash
# La build di P3 in build_radio, quando la S22 della base ha finito: non piu' di 6 lavori per sessione. A nice 10.
R=/home/randolph/Claude/melchior-688; B=$R/engine/build_radio; U=$B/base2
until [ -f "$U/fine_extra.txt" ]; do sleep 10; done
export TMPDIR=$B/tmp_g; mkdir -p "$TMPDIR"
date
cd "$B" && nice -n 10 "$R/tools/venv/bin/ninja" -j6 melchior688 prove serie_sandbox rilievo_missioni > "$B/costruzione_p3.log" 2>&1
echo "ninja rc $?"; date
grep -E "error|FAILED" "$B/costruzione_p3.log" | head -20 | cut -c1-240
