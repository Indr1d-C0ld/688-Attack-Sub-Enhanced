#!/bin/bash
# serie_ia.sh: sonda_ia sul binario di partenza, colpi vicini (un colpo a contatto perde il battello in un minuto e non dice niente): Mk 46 di traverso a 7, 15 e 30 m, a 50 e 150 m di quota
B=/home/randolph/Claude/melchior-688/engine/build_danni; export TMPDIR=$B/tmp; U=/home/randolph/Claude/melchior-688/tools/re/prove/isola/sonda_ia_prima.tsv; : > $U; cd $B/corse || exit 1
for modo in lato:7 lato:15 lato:30; do nice -n 19 $B/b_prima/sonda_ia $B/dati/motore mk46 los_angeles,sturgeon,trafalgar,ohio,oscar,victor3,alfa,akula $modo 12 1 50,150 >> $U 2>/dev/null; done
python3 -I - "$U" <<'PY'
import sys,collections
righe=[l.rstrip('\n').split('\t') for l in open(sys.argv[1]) if not l.startswith('#') and not l.startswith('arma')]
tot=collections.defaultdict(collections.Counter)
for r in righe:
    for k in ((r[3],r[1]),('tutti',r[1])):
        c=tot[k]; c['n']+=1; c[r[5]]+=1
        for i,n in zip(range(8,13),('regola','b_acqua','b_fuoco','f05','scram')): c[n]+=int(r[i])
        c['fumo08']+=float(r[13])>0.8
print('modo classe n | persi emersi menom in_quota | regola_ia | b_acqua b_fuoco f>0,5comando scram fumo>0,8')
for k,c in sorted(tot.items()): print(k[0],k[1],c['n'],'|',c['perso'],c['emerso'],c['menomato'],c['in_quota'],'|',c['regola'],'|',c['b_acqua'],c['b_fuoco'],c['f05'],c['scram'],c['fumo08'])
PY
