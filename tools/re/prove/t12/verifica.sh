#!/bin/bash
# Prima di misurare: casi e rilievo sulla build pulita, e due partite di P3 rigiocate col binario di adesso
R=/home/randolph/Claude/melchior-688; B=$R/engine/build_trailer1; V=$B/tmp/t12/verifica; mkdir -p $V/casi $V/ril $V/p3
export TMPDIR=$B/tmp LD_LIBRARY_PATH=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
cd $R/engine || exit 1
md5sum $B/melchior688 $B/serie_ab $B/serie_sandbox $B/prove $B/rilievo_missioni $R/data/motore/acustica.toml > $V/md5.txt
(CARTELLA=$V/casi nice -n 19 $R/tools/regressione/casi.sh $B/melchior688 > $V/casi.txt 2>&1; diff $R/tools/regressione/riferimento.txt $V/casi.txt > $V/casi.diff; echo "casi: diff $?" > $V/casi.esito) &
(CARTELLA=$V/ril nice -n 19 $R/tools/regressione/rilievo_classico.sh $B/rilievo_missioni > $V/rilievo.txt 2>&1; echo "rilievo $?" > $V/rilievo.esito) &
gioca() { n=$1; shift; nice -n 19 $B/melchior688 --sandbox "$@" --salva-traccia $V/p3/$n.m688tr --istantanea $V/p3/$n.bmp > $V/p3/$n.txt 2>&1; echo "$n rc $?" >> $V/p3/fatti.txt; }
grep -E '^(S03_1|I07_1) ' $R/engine/build_radio/base2/lavori_18.txt > $V/p3/lavori.txt
while read -r n resto; do gioca $n $resto & done < $V/p3/lavori.txt
wait
cat $V/casi.esito $V/rilievo.esito; cut -c1-200 $V/casi.diff | head -20; tail -2 $V/rilievo.txt | cut -c1-200
for n in S03_1 I07_1; do cmp $V/p3/$n.m688tr $R/engine/build_radio/p3/$n.m688tr | head -1; grep -h -E "impronta|ESITO" $V/p3/$n.txt | cut -c1-120; grep -h -E "impronta|ESITO" $R/engine/build_radio/p3/$n.txt | cut -c1-120; done
