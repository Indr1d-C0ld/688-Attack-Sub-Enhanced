#!/usr/bin/env python3
# T12: i dettagli che le righe di leggi_9.py e leggi_10.py non danno. Uso: leggi_dettagli.py SERIE
import re,subprocess,sys,math,statistics as st,collections as co
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
        p=l.split(); o[int(p[0])]=(p[1],float(p[2]),float(p[3]),float(p[4]),float(p[5]),int(p[6]))
    return o
def dist(a,b): return math.hypot(a[1]-b[1],a[2]-b[2])/1000
print('### SCENE1A')
for i in (1,2,3):
    T=f'{U}/SCENE1A/partita_{i}.m688tr'; r=reg(T)
    fuori=[]
    for e in r:
        if e['k']=='lancio_giocatore':
            p=posti(T,e['t']); fuori.append((round(e['t']),round(min(dist(p[0],v) for k,v in p.items() if v[0]=='spruance'),1)))
    sil=[l.split() for l in run(T,'ia','siluro').splitlines()]
    tipi=co.Counter(l[-1] for l in sil); sorg=co.Counter((l[-1],l[3]) for l in sil)
    rit=[e for e in r if e['k']=='ritirata_ia']
    sc=[e['x'] for e in r if e['k']=='scambio_tracce'][:3]
    print(i,'lanci/dist',fuori,'siluro',dict(tipi),dict(sorg),'ritirate',[round(e['t']) for e in rit][:3],sc, 'allarme',[round(e['t']) for e in r if e['k']=='allarme_siluro'])
    print('  fine corsa',[(round(e['t']),e['k'],e['x'][:50]) for e in r if 'mk48' in e['ac'] and e['k'] not in('lancio_giocatore','allarme_siluro','scambio_tracce')][:6])
print('### SCENE2A')
for i in (1,2,3):
    T=f'{U}/SCENE2A/partita_{i}.m688tr'; un=unita(T); r=reg(T)
    alfa=[k for k,c in un.items() if c=='alfa'][0]
    pri=[e for e in r if e['k']=='scoperta' and e['a']==alfa or e['k']=='scoperta' and e['u']==alfa][:1]
    p0=posti(T,1);
    dd=[(q,round(dist(posti(T,q)[0],posti(T,q)[alfa]),1)) for q in (1,60,105,300,600,900,1190)]
    print(i,'alfa id',alfa,'prima scoperta',[(round(e['t']),e['x'][:40],e['d']) for e in pri],'dist',dd,'perdite',[l for l in run(T,'ia','contatto_perso').splitlines() if f'u={alfa} altra=0' in l][:6])
    print('  scoperte giocatore',[(round(e['t']),e['c'],e['d']) for e in r if e['k']=='scoperta'][:5])
print('### SCENE0A')
for i in (1,2,3):
    T=f'{U}/SCENE0A/partita_{i}.m688tr'; r=reg(T); un=unita(T)
    print(i,'esp',[(round(e['t']),e['val'] if 'val' in e else '',e['x'][:40]) for e in r if e['k']=='esposizione_lunga'])
    print('  lanci',[round(e['t']) for e in r if e['k']=='lancio_giocatore'])
    print('  colpi',[(round(e['t']),e['c'],e['ac'],un.get(e['a'])) for e in r if e['k']=='colpo'])
    print('  fine_missile',[(round(e['t']),e['ac'],e['x'][:40]) for e in r if e['k']=='fine_missile'])
    print('  siluri evitati',[(round(e['t']),e['ac']) for e in r if e['k']=='siluro_evitato'])
    print('  lanci nemici',[(round(e['t']),e['c'],e['ac']) for e in r if e['k']=='lancio_nemico'][:14])
    print('  incagli, aff, fine',[(round(e['t']),e['k'],e['c'],e['x'][:30]) for e in r if e['k'] in('incaglio','affondamento','fine','danno_domato')])
    print('  com',[(round(e['t']),e['x'][14:60]) for e in r if e['k']=='comandante_ia'][-7:])
    for e in [e for e in r if e['k']=='colpo' and e['c']=='los_angeles'][:1]:
        p=posti(T,e['t']-1); print('  al primo colpo quota,vel',p[0][3],p[0][4],'t',round(e['t']))
print('### Mk 75 e colpi sul Victor')
for x,L,i in [('allerta_anticipata','A',4),('allerta_anticipata','A',9),('allerta_anticipata','B',4),('allerta_anticipata','B',9),('falso_ordine_rientro','B',4),('falso_ordine_rientro','B',10),('scoppio_nel_transito','B',4)]:
    T=f'{U}/{x}/{L}/partita_{i}.m688tr'; r=reg(T); un=unita(T)
    mk=[e for e in r if e['k']=='colpo' and e['ac']=='mk75']
    tir=co.Counter(e['a'] for e in mk); seg=sum('a segno' in e['x'] for e in mk); ber=co.Counter(un.get(e['u'],'?') for e in mk)
    vic=[k for k,c in un.items() if c.startswith('victor')][0]
    ko=[(round(e['t']),e['ac'],un.get(e['a'])) for e in r if e['k']=='colpo' and e['u']==vic and e['ac']!='mk75']
    lan=[e for e in r if e['k']=='lancio_ia' and e['ac']=='mk75']
    print(x,L,i,'colpi',len(mk),'a segno',seg,'bersagli',dict(ber),'tiratori',dict(tir),'lanci',co.Counter(e['u'] for e in lan),'da',round(mk[0]['t']) if mk else None,'a',round(mk[-1]['t']) if mk else None,'altri colpi sul Victor',ko,'testi',set(re.sub(r'\d+','N',e['x'])[:50] for e in mk[:50]))
    if mk:
        p=posti(T,mk[0]['t']); print('   Victor al primo Mk75 quota,vel',p[vic][3],p[vic][4])
print('### falso: reazioni')
tot=co.Counter(); pc=co.Counter(); rel=[]
for i in range(1,11):
    r=reg(f'{U}/falso_ordine_rientro/B/partita_{i}.m688tr')
    for e in r:
        if e['k']!='reazione_ia': continue
        m=re.search(r"(crede|ignora|chiede conferma|smaschera)",e['x'])
        if m:
            tot[m[1]]+=1; pc[(e['c'],m[1])]+=1
            if e['c']=='sh60b': rel.append((i,round(e['t']),m[1]))
print(tot,{k:v for k,v in pc.items() if k[0]=='perry'},'elicottero',rel)
for t in (1200,):
    rows=[]
    for s in range(1,11):
        ris=[]
        for L in 'AB':
            p=posti(f'{U}/falso_ordine_rientro/{L}/partita_{s}.m688tr',t)
            conv=[v for k,v in p.items() if v[0] in('mercantile','cargo','petroliera') and k<10000 and not v[5]]
            cx=sum(v[1] for v in conv)/len(conv); cy=sum(v[2] for v in conv)/len(conv)
            ris.append(max(math.hypot(v[1]-cx,v[2]-cy)/1000 for k,v in p.items() if v[0]=='perry' and not v[5]))
        rows.append(ris)
    print('Perry piu lontana',t,round(st.median(r[0] for r in rows),1),round(st.median(r[1] for r in rows),1),[ (round(a,1),round(b,1)) for a,b in rows])
print('### A: scoperta, datum seme 1')
T=f'{U}/allerta_anticipata/A/partita_1.m688tr'; r=reg(T)
print([(round(e['t']),e['k'],e['c'],e['x'][:60]) for e in r if e['k'] in('scoperta_ia','ingaggio_ia','allerta_forza') and e['t']<1000][:12])
print('### M3: fine dei P500')
for L in 'B':
    tot=co.Counter()
    for i in range(1,11):
        r=reg(f'{U}/spia_oltre_orizzonte/{L}/partita_{i}.m688tr')
        for e in r:
            if e['ac']=='p500': tot[(e['k'],re.sub(r'\d+','N',e['x'])[:36])]+=1
    print(tot)
for L in 'A':
    for i in (3,4,5,6,10):
        T=f'{U}/spia_oltre_orizzonte/{L}/partita_{i}.m688tr'; r=reg(T); un=unita(T)
        o=[]
        for e in r:
            if e['k'] in('lancio_ia','lancio_giocatore') and e['ac']=='mk48':
                p=posti(T,e['t']); o.append((round(e['t']),{v[0]:round(dist(p[0],v),0) for k,v in p.items() if k<10000 and v[0] in('krivak2','slava')}))
        print(i,o)
    print('primo solido Perry sullo Slava')
    for i in range(1,11):
        T=f'{U}/spia_oltre_orizzonte/{L}/partita_{i}.m688tr'; un=unita(T)
        sl=[k for k,c in un.items() if c=='slava'][0]
        pr=[k for k,c in un.items() if c=='perry']
        o=[l.split() for l in run(T,'ia','contatto_solido').splitlines() if f'altra={sl} ' in l and any(f'u={k} ' in l for k in pr)]
        print(i,[(o[j][0],o[j][2]) for j in range(min(3,len(o)))],'LA id',[k for k,c in un.items() if c=='los_angeles'])
print('### M4: Victor sul mercantile, A')
for x in ('quadro_m4_spia','quadro_m4_krivak'):
  for L in 'AB':
    v=[];vv=[]
    for i in range(1,11):
        T=f'{U}/{x}/{L}/partita_{i}.m688tr'; un=unita(T)
        vic=[k for k,c in un.items() if c.startswith('victor')][0]
        o=[l.split() for l in run(T,'ia','contatto_solido').splitlines() if f'u={vic} ' in l]
        m=[float(q[0]) for q in o if un.get(int(q[3][6:]),'')=='mercantile']
        v.append(round(m[0]) if m else None); vv.append(round(float(o[0][0])) if o else None)
    print(x,L,'mercantile',v,'primo',vv)
print('### S5: antenna per seme, nave_spia')
def toml(p):
    s=open(p).read(); return re.findall(r'antenna_media_s = ([\d.-]+),',s)
print(toml(f'{U}/nave_spia/A/serie.toml')[:10]); print(toml(f'{U}/nave_spia/B/serie.toml')[:10])
for x in ('allerta_anticipata','quadro_m4_spia','spia_oltre_orizzonte'):
    for L in 'AB':
        s=open(f'{U}/{x}/{L}/serie.toml').read()
        print(x,L,'nato contro_scoperte',re.findall(r'contro_scoperte = \{ media = ([\d.]+)',s)[:2],'lanci',re.findall(r'\nlanci = \{ media = ([\d.]+), dev = [\d.]+, min = ([\d.]+), max = ([\d.]+), q10 = [\d.]+, mediana = ([\d.]+)',s)[:2],'a_segno',re.findall(r'\na_segno = \{ media = ([\d.]+)',s)[:2],'perse',re.findall(r'\nperse = \{ media = ([\d.]+)',s)[:2], 'esiti',re.findall(r'"(\d+)" = \{ volte = (\d+)',s))
print('### Fermi')
for s in ('S10','S14','S15','S16','S19'):
    for i in (1,2,3):
        T=f'{U}/fermo/{s}_{i}.m688tr'; r=reg(T)
        ty=co.Counter(e['k'] for e in r)
        print(s,i,'dur',[e['t'] for e in r if e['k']=='fine'],[e['x'][:50] for e in r if e['k']=='fine'],{k:ty[k] for k in ('ping_subito','scoperta_ia','lancio_nemico','lancio_ia','lancio_giocatore','classificazione_ia','avvistato','affondamento') if ty[k]},'prima scoperta',[(round(e['t']),e['c'],e['d']) for e in r if e['k']=='scoperta_ia' and e['a']==0][:3],'bruciato?',[(round(e['t']),e['x'][:70]) for e in r if e['k'] in('messaggio_missione','fine_missione','script','obiettivo','sconfitta','vittoria')][:4])
