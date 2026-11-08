#!/usr/bin/env python3
# T12 §10: seme per seme e i totali di un esperimento sulla S5, dalle tracce A e B. Uso: leggi_10.py SERIE ESPERIMENTO
import re,subprocess,sys,statistics as st
U,x=sys.argv[1:3]; S=U+'/../sonda_t12'
def reg(t):
    out=[]
    for l in subprocess.run([S,t,'reg'],capture_output=True,text=True).stdout.splitlines():
        m=re.match(r'([\d.]+) (\d+) (\S+) u=(-?\d+) altra=(-?\d+) arma=(-?\d+) d=(-?\d+) classe=(\S*) arma_classe=(\S*) cod=(-?\d+) val=(\S+) \| (.*)',l)
        if m: out.append(dict(t=float(m[1]),k=m[3],u=int(m[4]),a=int(m[5]),arma=int(m[6]),c=m[8],ac=m[9],x=m[12]))
    return out
def unita(t):
    o={}
    for l in subprocess.run([S,t,'unita'],capture_output=True,text=True).stdout.splitlines():
        p=l.split()
        if p[0].isdigit(): o[int(p[0])]=p[1]
    return o
def partita(L,i):
    T=f'{U}/{x}/{L}/partita_{i}.m688tr'; r=reg(T); un=unita(T)
    vic=[k for k,c in un.items() if c.startswith('victor')]
    v=vic[0] if vic else -1
    d=dict(perse=0,vic=None,segno=0,mk=0,mks=0,mk0=None,mk1=None,lanci=0,l1=None,v=v,kv=[])
    for e in r:
        if e['k']=='affondamento':
            if e['u']==v: d['vic']=round(e['t'])
            elif un.get(e['u'],'') not in('',) and e['u']>=0: d['perse']+=1
        if e['k']=='colpo':
            if 'Mk75' in e['x'] or 'Mk 75' in e['x'] or e['ac']=='mk75':
                d['mk']+=1; d['mk0']=d['mk0'] if d['mk0'] is not None else round(e['t']); d['mk1']=round(e['t'])
                if 'a segno' in e['x']: d['mks']+=1
            elif e['u']!=v and 'a segno' in e['x'] and e['a']==v: d['segno']+=1
            elif e['u']==v and 'a segno' in e['x']: d['kv'].append((round(e['t']),e['ac']))
        if e['k']=='lancio_ia' and e['u']==v:
            d['lanci']+=1; d['l1']=d['l1'] if d['l1'] is not None else round(e['t'])
    return d
print('==',x)
print('seme | perse A B | a segno A B | lanci A B | 1 lancio A B | Victor aff A B | Mk75 A B (a segno) | colpi sul Victor')
tot={k:[0,0] for k in ['perse','segno','lanci','vic']}; l1=[]
for i in range(1,11):
    a=partita('A',i); b=partita('B',i)
    print(f"{i:2} | {a['perse']} {b['perse']} | {a['segno']} {b['segno']} | {a['lanci']} {b['lanci']} | {a['l1']} {b['l1']} | {a['vic']} {b['vic']} | {a['mk']}({a['mks']}) {b['mk']}({b['mks']}) | {a['kv'][:3]} {b['kv'][:3]}")
    for j,d in enumerate((a,b)):
        tot['perse'][j]+=d['perse'];tot['segno'][j]+=d['segno'];tot['lanci'][j]+=d['lanci'];tot['vic'][j]+= d['vic'] is not None
    l1.append(a['l1'])
print('tot',tot,'primo lancio A',min(v for v in l1 if v is not None) if any(v is not None for v in l1) else None,max(v for v in l1 if v is not None) if any(v is not None for v in l1) else None,'semi senza lancio',sum(v is None for v in l1))
