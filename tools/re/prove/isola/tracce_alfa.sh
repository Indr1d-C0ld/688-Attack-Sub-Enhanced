#!/bin/bash
# tracce_alfa.sh <uscita>: come tracce.sh, sulle missioni dell'Alfa della libreria, coi due binari di seguito
B=/home/randolph/Claude/melchior-688/engine/build_danni; U=$1; export TMPDIR=$B/tmp LD_LIBRARY_PATH=/home/randolph/Claude/melchior-688/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
cd $B/corse || exit 1
for A in prima lavoro; do mkdir -p "$U/$A"
  for x in "LIB_I02_PESCATORE_DI_TERIBERKA 1" "LIB_I02_PESCATORE_DI_TERIBERKA 2" "LIB_I06_PONTE_DI_EKATERINA 1"; do
    set -- $x; n=$1_$2
    nice -n 19 $B/b_$A/melchior688 --missione libreria/$1 --seme $2 --regola simulazione --secondi 600 --stazione 4 --urto-giocatore 200,20,90 --traffico no --salva-traccia "$U/$A/$n.m688tr" --istantanea "$U/$A/$n.bmp" 2>&1 | grep -E "impronta|ESITO" | sed "s|$U/$A/||" > "$U/$A/$n.txt"
  done
done
diff -r "$U/prima" "$U/lavoro" > "$U/tracce.diff" 2>&1; echo "tracce alfa: diff $?"; ls "$U/prima" | wc -l; cat "$U/prima/"*.txt | cut -c1-110
