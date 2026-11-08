#!/bin/bash
# verifica.sh: build_s22, rilievo, casi e prove della schermata dei comandi; a schermata e periscopio finiti
E=/home/randolph/Claude/melchior-688/engine; O=$E/build_s22/peri; cd $E || exit 1
export TMPDIR=$O PATH=/home/randolph/Claude/melchior-688/tools/venv/lib/python3.13/site-packages/cmake/data/bin:$PATH LD_LIBRARY_PATH=/home/randolph/Claude/melchior-688/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
nice -n 19 cmake --build build_s22 -j6 --target melchior688 rilievo_missioni prove 2>&1 | grep -E "error|FAILED|ninja: |Linking CXX executable" | cut -c1-200 | tail -5
cp build_s22/prove $O/prove_688; cp build_s22/melchior688 $O/melchior_dopo
( CARTELLA=$O/ril nice -n 19 ../tools/regressione/rilievo_classico.sh $E/build_s22/rilievo_missioni > $O/rilievo.txt 2>&1 ) &
( mkdir -p $O/casi; CARTELLA=$O/casi nice -n 19 ../tools/regressione/casi.sh $O/melchior_dopo > $O/casi.txt 2>&1 ) &
( cd $O && nice -n 19 ./prove_688 -tc="*dal vero: nell*,*dal vero: nel classico*,*la quota a schermo*,*schermata dei comandi*,*schermata comandi*,*manovra*,*periscop*,*comando*,*tasti*,*testi*" > $O/prove.txt 2>&1 ) &
wait
cut -c1-100 $O/rilievo.txt; diff $O/casi.txt ../tools/regressione/riferimento.txt | cut -c1-110 | head -24; grep -E "ERROR|test cases|Status" $O/prove.txt | cut -c1-170 | tail -12
