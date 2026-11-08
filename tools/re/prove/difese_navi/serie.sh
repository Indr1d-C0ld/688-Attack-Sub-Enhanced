#!/bin/bash
# Le serie prima/dopo delle difese delle navi (proposta.md §7), 10 semi a nice 19, una scena alla volta. Il binario e' una copia
# fissa, cosi' la build puo' ricompilare mentre gira. Uso: serie.sh ETICHETTA CARTELLA_DEI_BINARI
# Scene: T2 (Slava e Krivak contro una Perry, in pace), M3 con la spia (salve di P-500 sulle Perry, in guerra), S5 (non regressione).
R=/home/randolph/Claude/melchior-688
E=$1
B=$2
U=$R/engine/build_difese/tmp/serie/$E
mkdir -p "$U" || exit 1
export TMPDIR=$R/engine/build_difese/tmp LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
cd "$R/engine" || exit 1
md5sum "$B/melchior688" "$B/serie_sandbox" "$R/data/motore/missili.toml" "$R/data/motore/classi.toml" "$R/data/missioni/prove/PROVA_T2_MISSILI.lua" "$R/data/missioni/prove/PROVA_M3_ORIZZONTE.lua" > "$U/md5.txt"
date > "$U/inizio.txt"
S="nice -n 19 $B/serie_sandbox --semi 10 --da 1 --lavori 6 --motore $B/melchior688"
$S --uscita "$U/t2" -- --missione prove/PROVA_T2_MISSILI > "$U/t2.log" 2>&1
$S --uscita "$U/m3" -- --esperimento "$R/data/motore/esperimenti/spia_oltre_orizzonte.m688v" > "$U/m3.log" 2>&1
$S --uscita "$U/s5" --codice 8FZD3Q-4G0000-04K000-0000HZ-G002QH > "$U/s5.log" 2>&1
date > "$U/fine.txt"
