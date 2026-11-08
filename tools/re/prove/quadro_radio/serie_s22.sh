#!/bin/bash
# la S22 a 4 nodi, 12 semi (proposta.md §4), 4 alla volta. $1: etichetta; M688_QUADRO_SPENTO nell'ambiente per l'interruttore spento
R=/home/randolph/Claude/melchior-688; B=$R/engine/build_radio/fisso; U=$R/engine/build_radio/serie/s22_$1
mkdir -p "$U/tmp"; export TMPDIR=$U/tmp LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
cd "$R/engine" || exit 1
seq 1 12 | xargs -P 4 -I{} sh -c "nice -n 19 $B/sonda_s22 {} 4 > $U/seme_{}.txt 2>&1"
for s in $(seq 1 12); do tail -1 $U/seme_$s.txt | cut -c1-200; done > $U/esiti.txt
cat $U/esiti.txt
