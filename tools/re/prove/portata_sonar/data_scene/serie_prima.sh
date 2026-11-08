#!/bin/bash
# Le basi prima della correzione: M4 e S5, tre semi, col codice delle serie, senza e con la data del file. Nice 19.
R=/home/randolph/Claude/melchior-688; B=$R/engine/build_portata/prima; U=$R/tools/re/prove/portata_sonar/data_scene/esiti/serie_prima
export LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu TMPDIR=$R/engine/build_portata/tmp
mkdir -p "$U"; cd "$R/engine" || exit 1
M4=8CDXSF-P80000-04K000-0000HZ-G00295; S5=8FZD3Q-4G0000-04K000-0000HZ-G002QH
S="nice -n 19 $B/serie_sandbox --semi 3 --da 1 --lavori 3 --motore $B/melchior688"
$S --uscita "$U/m4_senza" --codice $M4 > "$U/m4_senza.log" 2>&1 &
$S --uscita "$U/m4_data" --codice $M4 -- --data 1989-06-12 --ora 11 > "$U/m4_data.log" 2>&1 &
wait
$S --uscita "$U/s5_senza" --codice $S5 > "$U/s5_senza.log" 2>&1 &
$S --uscita "$U/s5_data" --codice $S5 -- --data 1989-10-21 --ora 14 > "$U/s5_data.log" 2>&1 &
wait
echo fine
