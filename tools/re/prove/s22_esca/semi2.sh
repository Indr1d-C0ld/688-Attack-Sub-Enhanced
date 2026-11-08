#!/bin/bash
cd /home/randolph/Claude/melchior-688/engine; export TMPDIR=/home/randolph/Claude/melchior-688/engine/../tools/re/prove/s22_esca/tmp
for s in 5 6 7 8; do nice -n 5 build_chiusura/sonda_vtma $s 20 > /home/randolph/Claude/melchior-688/engine/../tools/re/prove/s22_esca/semi/vtma_${s}_20.txt 2>&1 & done
for s in 1 2 3 4 5 6 7 8; do nice -n 5 build_chiusura/sonda_vtma $s 4 > /home/randolph/Claude/melchior-688/engine/../tools/re/prove/s22_esca/semi/vtma_${s}_4.txt 2>&1 & done
for s in 5 6 7 8; do nice -n 5 build_chiusura/sonda_base $s 4 > /home/randolph/Claude/melchior-688/engine/../tools/re/prove/s22_esca/semi/base_${s}_4.txt 2>&1 & done
wait
cat /home/randolph/Claude/melchior-688/engine/../tools/re/prove/s22_esca/semi/vtma_*_20.txt; echo; cat /home/randolph/Claude/melchior-688/engine/../tools/re/prove/s22_esca/semi/vtma_*_4.txt; echo; cat /home/randolph/Claude/melchior-688/engine/../tools/re/prove/s22_esca/semi/base_*_4.txt
