#!/bin/bash
# Le cifre di T12 §9 e §10 e delle schede senza ordini, sulla build pulita di trailer-1. A nice 19, al massimo 6 alla volta.
R=/home/randolph/Claude/melchior-688; B=$R/engine/build_trailer1; U=$B/tmp/t12/serie; mkdir -p $U
export TMPDIR=$B/tmp LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
cd $B || exit 1
date "+inizio %F %T" >> $U/fatti.txt
# §9: le tre originali col comandante IA, 3 semi
for m in SCENE1A SCENE2A SCENE0A; do
  nice -n 19 ./serie_sandbox --semi 3 --lavori 3 --motore $B/melchior688 --uscita $U/$m -- --missione $m > $U/$m.log 2>&1
  echo "$m rc $? $(date +%T)" >> $U/fatti.txt
done
# le schede «senza ordini»: il giocatore fermo, 3 semi
cd $R/engine || exit 1
gioca() { n=$1; shift; nice -n 19 $B/melchior688 "$@" --regola simulazione --salva-traccia $U/fermo/$n.m688tr --istantanea $U/fermo/$n.bmp > $U/fermo/$n.txt 2>&1; echo "$n rc $? $(date +%T)" >> $U/fatti.txt; }
export -f gioca; export B U; mkdir -p $U/fermo
for s in "S10 LIB_S10_CAVO_DELLA_FREGATA 7200" "S14 LIB_S14_PEDINATORE_IN_ATTESA 9000" "S15 LIB_S15_ANCORAGGIO 10800" "S16 LIB_S16_SCALA 7200" "S19 LIB_S19_PIETRA_SANTA 10800"; do
  set -- $s
  for k in 1 2 3; do echo "$1_$k --missione libreria/$2 --seme $k --secondi $3"; done
done | xargs -P 6 -L 1 bash -c 'gioca "$@"' _
cd $B || exit 1
# §10 e la M4: gli esperimenti, 10 coppie ciascuno
for x in spia_oltre_orizzonte allerta_anticipata falso_ordine_rientro nave_spia scoppio_nel_transito quadro_m4_spia quadro_m4_krivak; do
  nice -n 19 ./serie_ab --esperimento ../../data/motore/esperimenti/$x.m688v --lavori 6 --uscita $U/$x > $U/$x.log 2>&1
  echo "$x rc $? $(date +%T)" >> $U/fatti.txt
done
date "+fine %F %T" >> $U/fatti.txt
