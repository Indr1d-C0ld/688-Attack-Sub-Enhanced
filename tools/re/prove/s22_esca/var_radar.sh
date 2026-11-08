#!/bin/bash
cd /home/randolph/Claude/melchior-688/engine
nice -n 5 /usr/bin/c++ -DTOML_ASSERT="(void)sizeof" -Imissioni/include -Inucleo/include -isystem terzi/tomlplusplus -isystem terzi/ymfm -isystem terzi/lua -O2 -DNDEBUG -std=c++20 -c build_chiusura/var/missione_senza_radar.cpp -o build_chiusura/var/missione_senza_radar.o || exit 1
/home/randolph/Claude/melchior-688/engine/../tools/re/prove/s22_esca/compila.sh sonda_vradar build_chiusura/var/missione_senza_radar.o || exit 2
export TMPDIR=/home/randolph/Claude/melchior-688/engine/../tools/re/prove/s22_esca/tmp
for s in 1 2 3 4 5 6 7 8; do nice -n 5 build_chiusura/sonda_vradar $s 20 > /home/randolph/Claude/melchior-688/engine/../tools/re/prove/s22_esca/semi/vradar_${s}_20.txt 2>&1 & done
for s in 1 2 3 4; do nice -n 5 build_chiusura/sonda_vradar $s 4 > /home/randolph/Claude/melchior-688/engine/../tools/re/prove/s22_esca/semi/vradar_${s}_4.txt 2>&1 & done
wait
cat /home/randolph/Claude/melchior-688/engine/../tools/re/prove/s22_esca/semi/vradar_*_20.txt; echo; cat /home/randolph/Claude/melchior-688/engine/../tools/re/prove/s22_esca/semi/vradar_*_4.txt
