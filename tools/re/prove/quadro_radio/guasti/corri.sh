#!/bin/bash
# ogni guasto col suo filtro: deve fallire. Prima, senza guasti, tutte le prove del quadro devono passare
B=/home/randolph/Claude/melchior-688/engine/build_radio
E=/home/randolph/Claude/melchior-688/tools/re/prove/quadro_radio/esiti/guasti.txt
cd $B; : > $E; export TMPDIR=$B/tmp_g
nice -n 10 ./prove_guasti -tc="quadro ai sottomarini*" 2>&1 | tail -3 >> $E
while read g f; do
  o=$(M688_GUASTO=$g nice -n 10 ./prove_guasti -tc="$f" 2>&1)
  esito=$(echo "$o" | grep -c "Status: FAILURE")
  echo "guasto $g filtro $f -> $([ $esito = 1 ] && echo ROSSA || echo VERDE-NON-VA)" >> $E
  echo "$o" | grep -E "ERROR:" | head -3 | cut -c50-200 >> $E
done <<'L'
1 *a 100 m non riceve*
71 *a 100 m non riceve*
72 *a 100 m non riceve*
2 *a 15 m senza albero*
3 *salito 10 s prima*
4 *oltre 10 nodi a quota*
41 *oltre 10 nodi a quota*
51 *sotto la banchisa*
52 *sotto la banchisa*
53 *sotto la banchisa*
54 *sotto la banchisa*
61 *il disturbo della parte*
62 *il disturbo della parte*
63 *il disturbo della parte*
87 *salita disturbata*
81 *sale alle finestre*
82 *sale alle finestre*
85 *sale alle finestre*
86 *sale alle finestre*
83 *esperto rimanda*
84 *esperto rimanda*
91 *il dato fresco*
99 *il dato fresco*
92 *contatto proprio e l'ordine*
96 *contatto proprio e l'ordine*
93 *sul punto senza contatto*
94 *sul punto senza contatto*
95 *sul punto senza contatto*
98 *sul punto senza contatto*
97 *in A3 il dato vecchio*
10 *nessun siluro*
11 *un rapporto su un aereo*
121 *interruttore spento*
122 *interruttore spento*
L
echo FINE >> $E
