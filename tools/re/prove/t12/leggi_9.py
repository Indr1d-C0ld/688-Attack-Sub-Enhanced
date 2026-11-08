#!/usr/bin/env python3
# T12 §9: le righe delle tre originali, dalle tracce della serie (serie.sh). Uso: leggi_9.py SERIE
import re,subprocess,sys
U=sys.argv[1]; S=U+'/../sonda_t12'
def reg(t):
    out=[]
    for l in subprocess.run([S,t,'reg'],capture_output=True,text=True).stdout.splitlines():
        m=re.match(r'([\d.]+) (\d+) (\S+) u=(-?\d+) altra=(-?\d+) arma=(-?\d+) d=(-?\d+) classe=(\S*) arma_classe=(\S*) cod=(-?\d+) val=(\S+) \| (.*)',l)
        if m: out.append(dict(t=float(m[1]),k=m[3],u=int(m[4]),a=int(m[5]),arma=int(m[6]),d=int(m[7]),c=m[8],ac=m[9],x=m[12]))
    return out
def esito(m,i):
    s=open(f'{U}/{m}/serie.toml').read()
    return re.findall(r'\[\[partita\]\]\nesito = (\d+)',s)
def tipi(r):
    d={}
    for e in r: d[e['k']]=d.get(e['k'],0)+1
    return d
for m in ['SCENE1A','SCENE2A','SCENE0A']:
    print('=====',m,'esiti',esito(m,0))
    for i in (1,2,3):
        T=f'{U}/{m}/partita_{i}.m688tr'; r=reg(T)
        print(f'-- seme {i}',tipi(r))
        sc=[e for e in r if e['k']=='scoperta_ia' and e['a']==0]
        print(' scoperto:',[(round(e['t']),e['c'],round(e['d'])) for e in sc][:8])
        la=[e for e in r if e['k']=='lancio_giocatore']
        print(' lanci:',[(round(e['t']),e['ac'],e['a'],round(e['d'])) for e in la])
        print(' colpi/fine:',[(round(e['t']),e['k'],e['c'],e['ac'],e['x'][:70]) for e in r if e['k'] in('colpo','colpo_subito','affondamento','fine','incaglio','giocatore_colpito','danno','perdita_battello','sconfitta','esca_rifiutata','aggancio','aggancio_siluro','fine_corsa')][:40])
        if m=='SCENE1A':
            print(' ritirate',sum(e['k']=='ritirata_ia' for e in r),'prima',next((round(e['t']) for e in r if e['k']=='ritirata_ia'),None),'scambi',sum(e['k']=='scambio_tracce' for e in r))
        if m=='SCENE2A':
            print(' comandante:',[(round(e['t']),e['x'][:60]) for e in r if e['k']=='comandante_ia'])
