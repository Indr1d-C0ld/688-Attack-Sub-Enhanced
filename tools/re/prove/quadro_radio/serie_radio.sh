#!/bin/bash
# Le serie del quadro radio (proposta.md §5): binario fisso in build_radio/fisso, 10 semi a nice 19, una scena alla volta
R=/home/randolph/Claude/melchior-688; B=$R/engine/build_radio/fisso; U=$R/engine/build_radio/serie; D=$R/engine/build_difese/dopo
mkdir -p "$U" "$U/tmp" || exit 1
export TMPDIR=$U/tmp LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
cd "$R/engine" || exit 1
md5sum $B/melchior688 $B/serie_sandbox $B/melchior688_spento $D/melchior688 $D/serie_sandbox > "$U/md5.txt"
S5=8FZD3Q-4G0000-04K000-0000HZ-G002QH; M4=8CDXSF-P80000-04K000-0000HZ-G00295; E=$R/data/motore/esperimenti
S="nice -n 19 $B/serie_sandbox --semi 10 --da 1 --lavori 6 --motore $B/melchior688"
date > "$U/inizio.txt"
# i controlli di riuso: la spia sul binario dopo e il quadro spento devono dare la s5 di dopo, partita per partita
nice -n 19 $D/serie_sandbox --semi 2 --da 1 --lavori 2 --motore $D/melchior688 --uscita "$U/ctl_dopo_spia" -- --esperimento $E/nave_spia.m688v > "$U/ctl_dopo_spia.log" 2>&1 &
nice -n 19 $B/serie_sandbox --semi 2 --da 1 --lavori 2 --motore $B/melchior688_spento --uscita "$U/ctl_spento_s5" --codice $S5 > "$U/ctl_spento_s5.log" 2>&1 &
wait
date > "$U/controlli_fatti.txt"
$S --uscita "$U/m4_a" --codice $M4 > "$U/m4_a.log" 2>&1
$S --uscita "$U/m4_b" -- --esperimento $E/quadro_m4_spia.m688v > "$U/m4_b.log" 2>&1
$S --uscita "$U/m4_c" -- --esperimento $E/quadro_m4_krivak.m688v > "$U/m4_c.log" 2>&1
date > "$U/m4_fatta.txt"
$S --uscita "$U/m3" -- --esperimento $E/spia_oltre_orizzonte.m688v > "$U/m3.log" 2>&1
$S --uscita "$U/s5" --codice $S5 > "$U/s5.log" 2>&1
$S --uscita "$U/s5_spia" -- --esperimento $E/nave_spia.m688v > "$U/s5_spia.log" 2>&1
date > "$U/fine.txt"
