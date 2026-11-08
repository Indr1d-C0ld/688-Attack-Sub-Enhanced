#!/bin/bash
# regressione.sh: tutta la prova d'identita' del passo 1 sulle copie private, quattro processi in tutto, a nice 19
B=/home/randolph/Claude/melchior-688/engine/build_danni; S=/home/randolph/Claude/melchior-688/tools/re/prove/isola; R=/home/randolph/Claude/melchior-688/tools/regressione
U=$B/corse/regressione; rm -rf "$U"; mkdir -p "$U"
( CARTELLA=$U/rilievo nice -n 19 $R/rilievo_classico.sh $B/b_lavoro/rilievo_missioni > "$U/rilievo.txt" 2>&1; echo fine rilievo ) &
( $S/tracce.sh prima "$U/tp" > /dev/null; $S/tracce.sh lavoro "$U/tl" > /dev/null; diff -r "$U/tp" "$U/tl" > "$U/tracce.diff" 2>&1; echo fine tracce $? ) &
( mkdir -p "$U/cp" "$U/cl"; CARTELLA=$U/cp nice -n 19 $R/casi.sh $B/b_prima/melchior688 > "$U/casi_prima.txt" 2>&1; CARTELLA=$U/cl nice -n 19 $R/casi.sh $B/b_lavoro/melchior688 > "$U/casi_lavoro.txt" 2>&1; diff "$U/casi_prima.txt" "$U/casi_lavoro.txt" > "$U/casi.diff"; echo fine casi $? ) &
( CARTELLA=$U/suite nice -n 19 $R/suite.sh $B/corse/prove_688 > "$U/suite.txt" 2>&1; echo fine suite ) &
wait
