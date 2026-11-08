#!/bin/bash
cd /home/randolph/Claude/melchior-688/engine; export TMPDIR=/home/randolph/Claude/melchior-688/engine/build_chiusura/tmp
for p in prova_s22 mut_velocita mut_regola; do mkdir -p $TMPDIR/$p; TMPDIR=$TMPDIR/$p nice -n 5 build_chiusura/$p '-tc=libreria S22*' -s > /home/randolph/Claude/melchior-688/engine/../tools/re/prove/s22_esca/verifica_$p.txt 2>&1 & done
wait
for p in prova_s22 mut_velocita mut_regola; do echo "== $p"; grep -E "ERROR|S22 a|S22: i due|test cases|assertions|values:" /home/randolph/Claude/melchior-688/engine/../tools/re/prove/s22_esca/verifica_$p.txt | cut -c60-190; done
