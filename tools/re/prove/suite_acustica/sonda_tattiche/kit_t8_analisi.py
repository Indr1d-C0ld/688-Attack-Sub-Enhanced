import csv,glob,statistics as st,collections as C
D='tools/re/prove/suite_acustica/sonda_tattiche/'
R=[]
for f in sorted(glob.glob(D+'kit_t8_partita_n*_s1.tsv')):
    n=f.split('_n')[1].split('_')[0]; re=f.split('_')[-2]
    for r in csv.DictReader(open(f),delimiter='\t'):
        r['nodi']=n; r['re']=re
        for k in 'tiro preso t_tiro_s distanza_lancio_m t_esito_s d_min_m t_allarme_s d_allarme_m quota_lancio_m esca'.split(): r[k]=float(r[k])
        R.append(r)
print('partite',len(R),'con tiro',sum(r['tiro']>0 for r in R),'prese',sum(r['preso']>0 for r in R))
T=[r for r in R if r['tiro']>0]
print('per nodi',{n:(sum(1 for r in R if r['nodi']==n),sum(1 for r in T if r['nodi']==n)) for n in ('5','20')})
def fascia(d): return '<1' if d<1000 else '1-2' if d<2000 else '2-4' if d<4000 else '4-8' if d<8000 else '>8'
FS=['<1','1-2','2-4','4-8','>8']
print('\nlanci per fascia (km):',C.Counter(fascia(r['distanza_lancio_m']) for r in T))
print('t_tiro min/med/max',min(r['t_tiro_s'] for r in T),st.median(r['t_tiro_s'] for r in T),max(r['t_tiro_s'] for r in T))
for re in ('subito','allarme'):
  print('\nreazione',re,'  casella = presi/tiri')
  print('manovra'.ljust(12)+''.join(f.rjust(9) for f in FS)+'    totale')
  for m in ('niente','A','B','C','C+esca','A+soffitto'):
    s=m.ljust(12); tp=tt=0
    for f in FS:
        x=[r for r in T if r['re']==re and r['manovra']==m and fascia(r['distanza_lancio_m'])==f]
        p=sum(r['preso']>0 for r in x); tp+=p; tt+=len(x)
        s+=(f'{p}/{len(x)}' if x else '-').rjust(9)
    print(s+f'    {tp}/{tt}')
print('\nallarme: ritardo dal lancio e distanza del siluro, per fascia')
A=[r for r in T if r['t_allarme_s']>=0]
print('con allarme',len(A),'senza',len(T)-len(A))
for f in FS:
    x=[r for r in A if fascia(r['distanza_lancio_m'])==f]
    if x: print(f.ljust(5),len(x),'rit med %.0f s (%.0f-%.0f), d siluro med %.0f m'%(st.median(r['t_allarme_s']-r['t_tiro_s'] for r in x),min(r['t_allarme_s']-r['t_tiro_s'] for r in x),max(r['t_allarme_s']-r['t_tiro_s'] for r in x),st.median(r['d_allarme_m'] for r in x)))
print('\nsenza allarme, presi:',sum(r['preso']>0 for r in T if r['t_allarme_s']<0),'su',sum(1 for r in T if r['t_allarme_s']<0))
print('\nper nodi e reazione (presi/tiri)')
for n in ('5','20'):
  for re in ('subito','allarme'):
    print(n,re,{m:'%d/%d'%(sum(r['preso']>0 for r in T if r['nodi']==n and r['re']==re and r['manovra']==m),sum(1 for r in T if r['nodi']==n and r['re']==re and r['manovra']==m)) for m in ('niente','A','B','C','C+esca','A+soffitto')})
print('\nper rilevamento relativo al lancio, solo niente/C')
for m in ('niente','C'):
  for a,b,l in ((0,60,'prua'),(60,120,'traverso'),(120,181,'poppa')):
    x=[r for r in T if r['manovra']==m and a<=min(float(r['ril_lancio']),360-float(r['ril_lancio']))<b]
    print(m,l,'%d/%d'%(sum(r['preso']>0 for r in x),len(x)))
print('\nesca lanciata',sum(r['esca']>0 for r in T if r['manovra']=='C+esca'),'su',sum(1 for r in T if r['manovra']=='C+esca'))
print('A+soffitto: quota al lancio',C.Counter(r['quota_lancio_m'] for r in T if r['manovra']=='A+soffitto'))
print('t_esito-t_tiro niente, per fascia')
for f in FS:
    x=[r for r in T if r['manovra']=='niente' and fascia(r['distanza_lancio_m'])==f]
    if x: print(f,'%.0f s'%st.median(r['t_esito_s']-r['t_tiro_s'] for r in x),'d_min med %.0f'%st.median(r['d_min_m'] for r in x))
