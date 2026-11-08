#!/bin/bash
# costruisce le prove in build_radio e lancia il filtro $1, registro in esiti/$2.txt
R=/home/randolph/Claude/melchior-688
cd $R/engine/build_radio || exit 1
nice -n 10 $R/tools/venv/bin/ninja prove > costruzione.log 2>&1 || { tail -40 costruzione.log | cut -c1-300; exit 1; }
nice -n 10 ./prove -tc="$1" -d 2>&1 | tee $R/tools/re/prove/quadro_radio/esiti/$2.txt | cut -c1-260
