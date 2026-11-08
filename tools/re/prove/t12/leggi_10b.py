#!/usr/bin/env python3
# T12 §10, il dettaglio: affondamenti per classe, Mk 75, primo punto di divergenza, chi crede al falso ordine, le righe della M3.
# Uso: leggi_10b.py SERIE
import re,subprocess,sys,math,statistics as st
U=sys.argv[1]; S=U+'/../sonda_t12'
def run(*a): return subprocess.run([S,*a],capture_output=True,text=True).stdout
def reg(t):
    out=[]
    for l in run(t,'reg').splitlines():
        m=re.match(r'([\d.]+) (\d+) (\S+) u=(-?\d+) altra=(-?\d+) arma=(-?\d+) d=(-?\d+) classe=(\S*) arma_classe=(\S*) cod=(-?\d+) val=(\S+) \| (.*)',l)
        if m: out.append(dict(t=float(m[1]),k=m[3],u=int(m[4]),a=int(m[5]),arma=int(m[6]),d=int(m[7]),c=m[8],ac=m[9],x=m[12]))
    return out
def unita(t):
    o={}
    for l in run(t,'unita').splitlines():
        p=l.split()
        if p[0].isdigit(): o[int(p[0])]=p[1]
    return o
def posti(t,q):
    o={}
    for l in run(t,'posti',str(q)).splitlines():
        p=l.split(); o[int(p[0])]=(p[1],float(p[2]),float(p[3]),float(p[5]),int(p[6]))
    return o
ESP=['allerta_anticipata','falso_ordine_rientro','nave_spia','scoppio_nel_transito']
for x in ESP:
    print('==',x)
    tot={}
    for L in 'AB':
        vit=0; kl={}; perse=0; lanci_v=0; segno_v=0
        for i in range(1,11):
            T=f'{U}/{x}/{L}/partita_{i}.m688tr'; r=reg(T); un=unita(T)
            aff=[(round(e['t']),un.get(e['u'],'?')) for e in r if e['k']=='affondamento']
            lv=[e for e in r if e['k']=='lancio_ia' and e['c'].startswith('victor')]
            sv=[e for e in r if e['k']=='colpo' and 'a segno' in e['x'] and un.get(e['a'],'').startswith('victor')]
            lanci_v+=len(lv); segno_v+=len(sv)
            mk=[e for e in r if e['k'] in('artiglieria_salva',)]
            print(f' {L}{i}: aff {aff} | lanci Victor {len(lv)} a segno {len(sv)}')
            for t,c in aff: kl[c]=kl.get(c,0)+1
        print(f' {L} tot affondati {kl} lanci Victor {lanci_v} a segno {segno_v}')
# l'SH-60B e chi crede al falso ordine
tipi={}
for L in 'AB':
    for i in range(1,11):
        for e in reg(f'{U}/falso_ordine_rientro/{L}/partita_{i}.m688tr'):
            if L=='B' and any(w in e['x'].lower() for w in('falso','rientro','conferma','smasch')):
                tipi.setdefault((e['k'],re.sub(r'\d+','N',e['x'])[:60]),[]).append((i,round(e['t']),e['c']))
for k,v in sorted(tipi.items(),key=lambda a:-len(a[1]))[:20]:
    print(k,len(v),v[:3])
for t in (600,1200):
    rows=[]
    for s in range(1,11):
        ris=[]
        for L in 'AB':
            p=posti(f'{U}/falso_ordine_rientro/{L}/partita_{s}.m688tr',t)
            conv=[v for k,v in p.items() if k<10000 and v[0] not in('perry','victor3','los_angeles','sh60b','mk48','mk46','harpoon_84c','esca') and not v[4] and v[0] not in('type53','set65','tasm')]
            cx=sum(v[1] for v in conv)/len(conv); cy=sum(v[2] for v in conv)/len(conv)
            per=[math.hypot(v[1]-cx,v[2]-cy)/1000 for k,v in p.items() if v[0]=='perry' and not v[4]]
            ris.append(max(per))
        rows.append(ris)
    print('Perry piu\' lontana dal centro a',t,'A',round(st.median(r[0] for r in rows),1),'B',round(st.median(r[1] for r in rows),1))
# M3
print('== spia_oltre_orizzonte')
for L in 'AB':
    for i in range(1,11):
        T=f'{U}/spia_oltre_orizzonte/{L}/partita_{i}.m688tr'; r=reg(T); un=unita(T)
        p500=[round(e['t']) for e in r if e['k']=='lancio_ia' and e['ac']=='p500']
        ll=[(round(e['t']),e['c'],e['ac']) for e in r if e['k'] in('lancio_ia','lancio_giocatore') and e['ac']!='p500']
        segno=sum(1 for e in r if e['k']=='colpo' and e['ac']=='p500' and 'a segno' in e['x'])
        gua=sum(1 for e in r if 'guasta' in e['x']); abb=sum(1 for e in r if 'abbatt' in e['x'])
        aff=[(round(e['t']),un.get(e['u'],'?')) for e in r if e['k']=='affondamento']
        print(L,i,'p500',len(p500),p500[:1],'segno',segno,'guasti',gua,'abb',abb,'aff',aff,'altri',len(ll),ll[:3],'fine',[e['x'] for e in r if e['k']=='fine'][:1])
