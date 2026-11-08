#!/bin/bash
# ogni guasto col suo filtro: deve fallire. Prima, senza guasti, le prove dei rapporti devono passare
R=/home/randolph/Claude/melchior-688; B=$R/engine/build_radio; E=$R/tools/re/prove/quadro_radio/guasti_p3/esiti.txt
cd "$B" || exit 1; : > "$E"; export TMPDIR=$B/tmp_g LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
nice -n 10 ./prove_guasti_p3 -tc="rapporti dei sottomarini*" 2>&1 | tail -3 >> "$E"
while read -r g f; do
  o=$(M688_GUASTO=$g nice -n 10 ./prove_guasti_p3 -tc="$f" 2>&1)
  esito=$(echo "$o" | grep -c "Status: FAILURE")
  echo "guasto $g filtro $f -> $([ "$esito" = 1 ] && echo ROSSA || echo VERDE-NON-VA)" >> "$E"
done <<'L'
201 *riferisce a quota d'antenna*
202 *non sposta il caso*
203 *il fix arriva*
204 *il fix arriva*
205 *il giocatore della parte*
206 *nelle originali un battello*
207 *il fix arriva*
209 *il fix arriva*
L
echo FINE >> "$E"
