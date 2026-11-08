# Per il cancello di proposta.md §9: dalle tracce gia' giocate, quanto ogni sottomarino dell'IA e' esposto (albero, testa,
# emerso), quanto del tempo viene dalle salite d'ascolto, le emissioni radar, e chi lo scopre con radar, ESM, vista. Solo lettura.
import sys,re,pathlib,collections,multiprocessing
B=sys.argv[1] if len(sys.argv)>1 else '/home/randolph/Claude/melchior-688/engine/build_radio'
G=[('misura',f'{m}_{s}') for m in 'I02 I05 S08 S11'.split() for s in (1,2)]+[('misura_lunghe',f'{m}_{s}') for m in 'I06 I07 S03 S07 S15'.split() for s in (1,2)]
G+=[('misura',n) for n in 'm4a_1 m4a_2 m4c_2 m4c_6 s5_1 s5_3 s5_7'.split()]
if len(sys.argv)>2: G=[(sys.argv[2],n) for n in sys.argv[3:]]   # occasioni_tracce.py radice cartella partite...
def leggi(path):
    d=pathlib.Path(path).read_bytes(); p=7
    def u(d,p):
        v=k=0
        while True:
            c=d[p]; p+=1; v|=(c&0x7f)<<k; k+=7
            if not c&0x80: return v,p
    n,p=u(d,p); testo=d[p:p+n].decode('utf8','replace'); p+=n
    schemi={};delta='U C'
    for r in testo.splitlines():
        if r.startswith('record '):
            a=r.split(); schemi[a[1]]=[(x.split(':')[0],x.split(':')[1][0],(float(x.split(':')[1][1:]) if x.split(':')[1][1:] else 1.0)) for x in a[2:]]
        elif r.startswith('delta='): delta=r[6:]
    nomi={t:{c[0]:i for i,c in enumerate(s)} for t,s in schemi.items()}
    ang={t:[c[1]=='a' for c in s] for t,s in schemi.items()}
    fissi={t:[c[1] in 'ks' for c in s] for t,s in schemi.items()}
    prima={}; st={}; fl={}; cat={}; serie=collections.defaultdict(list); em={}; K=[]; I=collections.Counter(); poss=[]
    g_id=None
    while p<len(d):
        if d[p]!=0x46: break
        p+=1; n,p=u(d,p); e=p+n; q=p; p=e
        tt,q=u(d,q); t=tt/1000.0
        z,q=u(d,q); ch,q=u(d,q)
        if ch&1: prima.clear()
        vis={}
        while q<e:
            tag=chr(d[q]); q+=1
            if tag=='T':
                k,q=u(d,q); ln,q=u(d,q); st[k]=d[q:q+ln].decode('utf8','replace'); q+=ln; continue
            quanti,q=u(d,q); sc=schemi[tag]; nc=len(sc)
            for _ in range(quanti):
                g=[0]*nc
                for i in range(nc):
                    v,q=u(d,q); g[i]=(v>>1)^-(v&1)
                if tag in delta:
                    key=(tag,g[nomi[tag]['id']] if 'id' in nomi[tag] else 0)
                    pr=prima.get(key)
                    if pr is not None and len(pr)==nc:
                        for i in range(nc):
                            if fissi[tag][i]: continue
                            g[i]+=pr[i]
                            if ang[tag][i]:
                                giro=round(360.0/sc[i][2]); g[i]%=giro
                    prima[key]=g
                if tag=='N':
                    N=nomi['N']; fl[g[N['id']]]=g[N['flag']]; cat[g[N['id']]]=st.get(g[N['categoria']],'')
                elif tag=='V':
                    V=nomi['V']; em[g[V['id']]]=g[V['emissioni']]
                elif tag=='U':
                    U=nomi['U']; i=g[U['id']]
                    if cat.get(i)=='sottomarino' and fl.get(i,0)&0x18 in (8,0x10): vis[i]=(g[U['quota']]*0.1,g[U['alberi']],g[U['stato']])
                elif tag=='K':
                    k=nomi['K']; K.append((g[k['tempo']]*0.1,g[k['osservatore']],g[k['bersaglio']],st.get(g[k['sensore']],''),g[k['perso']]))
                elif tag=='I':
                    k=nomi['I']; tipo=st.get(g[k['tipo']],''); I[tipo]+=1
                    if tipo=='possibile': poss.append((g[k['tempo']]*0.1,g[k['unita']],g[k['altra']],st.get(g[k['s']],'')))
        for i,(qu,al,sta) in vis.items(): serie[i].append((t,qu,al,em.get(i,0),sta))
    return serie,K,I,poss
def lavora(a):
    d,n=a; serie,K,I,poss=leggi(f'{B}/{d}/{n}.m688tr')
    sal=collections.defaultdict(list)
    for l in pathlib.Path(f'{B}/{d}/{n}.txt').read_text(errors='replace').splitlines():
        if l.startswith('QUADRO salita'):
            k=dict(x.split('=') for x in l.split()[2:] if '=' in x); sal[int(k['id'])].append((float(k['t']),float(k['prossimo'])))
    r=[]
    for i,s in sorted(serie.items()):
        if i==0: continue
        c=collections.Counter(); prev=None; seg_asc=False; t0=s[0][0]
        for j,(t,qu,al,e,sta) in enumerate(s):
            if sta&3: continue
            dt=(s[j+1][0]-t) if j+1<len(s) else 0
            esp=(al&6)!=0 or qu<0.5
            if esp and not prev: seg_asc=any(a-1<=t<=b+300 for a,b in sal[i])
            prev=esp
            if esp: c['asc' if seg_asc else 'suo']+=dt
            if e: c['em_prof' if qu>20 else 'em_alto']+=dt
            c['tot']+=dt
        rilevati=collections.Counter(); per=collections.Counter()
        visti=set()
        for (t,o,b,sn,pe) in K:
            if b==i and sn in('radar','esm','vista','periscopio') and (o,sn) not in visti:
                visti.add((o,sn)); rilevati[('G' if o==0 else 'IA')+':'+sn]+=1
        for (t,o,b,sn) in poss:
            if b==i and sn in ('radar','esm','vista','periscopio'): per[('G' if o==0 else 'IA')+':'+sn]+=1
        T=max(c['tot'],1)
        r.append(f"{n:7} id{i:>2} dur {int(T):6}s | esposto suo {int(c['suo']):6}s ascolto {int(c['asc']):5}s | radar acceso prof {int(c['em_prof']):6}s alto {int(c['em_alto']):5}s | scoperte {dict(rilevati)} | possibili {dict(per)}")
    r.append(f"{n:7} tipi I: {dict(I)}")
    return r
if __name__=='__main__':
    with multiprocessing.Pool(6) as p:
        for r in p.imap(lavora,G):
            print('\n'.join(r),flush=True)
