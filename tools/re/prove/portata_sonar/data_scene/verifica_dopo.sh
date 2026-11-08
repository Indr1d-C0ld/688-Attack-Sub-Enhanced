#!/bin/bash
# La verifica dopo la correzione della data: le 23 scene e M4, S5 sul binario nuovo, poi casi e rilievo del classico. Nice 19 per le serie.
R=/home/randolph/Claude/melchior-688; D=$R/engine/build_portata/dopo; P=$R/tools/re/prove/portata_sonar/data_scene; E=$P/esiti
export LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu TMPDIR=$R/engine/build_portata/tmp
BIN=$D/melchior688 GIRO=$E/dopo_senza LAVORI=6 bash "$P/giro_scene.sh"
U=$E/serie_dopo; mkdir -p "$U"; cd "$R/engine" || exit 1
M4=8CDXSF-P80000-04K000-0000HZ-G00295; S5=8FZD3Q-4G0000-04K000-0000HZ-G002QH
S="nice -n 19 $D/serie_sandbox --semi 3 --da 1 --lavori 3 --motore $D/melchior688"
$S --uscita "$U/m4" --codice $M4 > "$U/m4.log" 2>&1 &
$S --uscita "$U/s5" --codice $S5 > "$U/s5.log" 2>&1 &
wait
mkdir -p "$E/casi" "$E/rilievo"
CARTELLA=$E/casi nice -n 15 bash "$R/tools/regressione/casi.sh" "$D/melchior688" > "$E/casi_dopo.txt" 2>&1
diff "$R/tools/regressione/riferimento.txt" "$E/casi_dopo.txt" > "$E/casi_dopo.diff"; echo "casi: diff rc $? ($(wc -l < "$E/casi_dopo.diff") righe)"
CARTELLA=$E/rilievo nice -n 15 bash "$R/tools/regressione/rilievo_classico.sh" "$D/rilievo_missioni" 2>&1 | tail -3
echo fine
