#!/bin/bash
# Il confronto di C, 3 semi a nice 19 sulle scene dove un deposito puo' finire: S07 (AK-630 della Kara), SCENE0A (Kirov, Kara, Slava), S5 e S12 (i cannoni).
# Serve per l'elenco di che cosa rimisura T12, non per le frequenze. uso: serie_colpi.sh ETICHETTA [senza_s07] (i binari in build_difese/foto/ETICHETTA)
R=/home/randolph/Claude/melchior-688; E=$1; B=$R/engine/build_difese/foto/$E; U=$R/engine/build_difese/tmp/colpi/$E
mkdir -p "$U/tmp" || exit 1
export TMPDIR=$U/tmp LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
cd "$R/engine" || exit 1
S="nice -n 19 $B/serie_sandbox --semi 3 --da 1 --lavori 4 --motore $B/melchior688"
[ "$2" = senza_s07 ] || $S --uscita "$U/s07" -- --missione libreria/LIB_S07_TAPPO > "$U/s07.log" 2>&1
$S --uscita "$U/s12" -- --missione libreria/LIB_S12_GRUPPO_CACCIATORE > "$U/s12.log" 2>&1
$S --uscita "$U/0a" -- --missione SCENE0A > "$U/0a.log" 2>&1
$S --uscita "$U/s5" --codice 8FZD3Q-4G0000-04K000-0000HZ-G002QH > "$U/s5.log" 2>&1
date > "$U/fine.txt"
