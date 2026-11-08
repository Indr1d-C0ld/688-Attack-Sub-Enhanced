#!/bin/bash
# le due prove nuove senza guasti, poi ogni guasto del ramo: deve fallire
R=/home/randolph/Claude/melchior-688; B=$R/engine/build_radio; E=$R/tools/re/prove/quadro_radio/guasti_p3/esiti_aereo.txt
cd "$B" || exit 1; : > "$E"; export TMPDIR=$B/tmp_g LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
A='*aereo ASW della parte*'; M="*nello stesso minuto*"
nice -n 10 ./prove_guasti_p3 -tc="$A" 2>&1 | tail -2 | head -1 >> "$E"
nice -n 10 ./prove_guasti_p3 -tc="$M" 2>&1 | tail -2 | head -1 >> "$E"
while read -r g f; do
  [ "$f" = A ] && p=$A || p=$M
  o=$(M688_GUASTO=$g nice -n 10 ./prove_guasti_p3 -tc="$p" 2>&1)
  echo "guasto $g -> $(echo "$o" | grep -c 'Status: FAILURE' | sed 's/^1$/ROSSA/; s/^0$/VERDE-NON-VA/') $(echo "$o" | grep -m1 -E 'ERROR' | cut -c60-170)" >> "$E"
done <<'L'
208 M
210 A
211 A
212 A
213 A
214 A
215 A
L
echo FINE >> "$E"
