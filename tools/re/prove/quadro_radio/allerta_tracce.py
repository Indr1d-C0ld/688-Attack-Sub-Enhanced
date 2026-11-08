# Per il capitolo 9 di proposta.md: dalle tracce gia' giocate, l'allerta di ogni sottomarino dell'IA (record V), il tempo
# in A2+ e nei limiti del filo, e le salite dei file QUADRO divise per allerta. Solo lettura, senza compilare.
# Uso: allerta_tracce.py [cartella_base_build]. I numeri valgono per il codice che ha giocato quelle tracce.
import sys,re,pathlib,bisect,collections,multiprocessing
B=sys.argv[1] if len(sys.argv)>1 else '/home/randolph/Claude/melchior-688/engine/build_radio'
G=[('misura',f'{m}_{s}') for m in 'I02 I05 S08 S11'.split() for s in (1,2)]+[('misura_lunghe',f'{m}_{s}') for m in 'I06 I07 S03 S07 S15'.split() for s in (1,2)]
G+=[('misura',n) for n in 'm4a_1 m4a_2 m4c_2 m4c_6 s5_1 s5_3 s5_7'.split()]
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
    prima={}; stringhe={}; fl={}; cat={}; serie=collections.defaultdict(list); al={}
    while p<len(d):
        if d[p]!=0x46: break
        p+=1; n,p=u(d,p); e=p+n; q=p; p=e
        tt,q=u(d,q); t=tt/1000.0
        z,q=u(d,q)
        ch,q=u(d,q)
        if ch&1: prima.clear()
        vis={}
        while q<e:
            tag=chr(d[q]); q+=1
            if tag=='T':
                k,q=u(d,q); ln,q=u(d,q); stringhe[k]=d[q:q+ln].decode('utf8','replace'); q+=ln; continue
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
                    N=nomi['N']; fl[g[N['id']]]=g[N['flag']]; cat[g[N['id']]]=stringhe.get(g[N['categoria']],'')
                elif tag=='V':
                    V=nomi['V']; al[g[V['id']]]=g[V['allerta']]
                elif tag=='U':
                    U=nomi['U']; i=g[U['id']]
                    if cat.get(i)=='sottomarino' and fl.get(i,0)&0x18 in (8,0x10): vis[i]=(g[U['quota']]*0.1,abs(g[U['velocita']])*0.01,g[U['stato']])
        for i,(qu,ve,st) in vis.items(): serie[i].append((t,qu,ve,al.get(i,-1)))
    return serie
def lavora(a):
    d,n=a; serie=leggi(f'{B}/{d}/{n}.m688tr')
    sal=collections.defaultdict(list); usa=collections.defaultdict(list); cop=collections.defaultdict(list)
    for l in pathlib.Path(f'{B}/{d}/{n}.txt').read_text(errors='replace').splitlines():
        if l.startswith('QUADRO '):
            k=dict(x.split('=') for x in l.split()[2:] if '=' in x); w=l.split()[1]
            if 'id' not in k: continue
            if w=='salita': sal[int(k['id'])].append(float(k['t']))
            elif w=='usato': usa[int(k['id'])].append(float(k['t']))
    def al_t(s,t):
        i=bisect.bisect_right([x[0] for x in s],t)-1
        return s[max(i,0)][3]
    r=[]
    for i,s in sorted(serie.items()):
        dur=len(s) or 1; a=[x[3] for x in s]
        f12=sum(1 for x in s if x[3]>=2 and x[1]<=120 and x[2]<=12); alto=sum(1 for x in a if x>=2)
        c=collections.Counter()
        for t in sal[i]:
            c['alta' if al_t(s,t)>=2 else 'bassa']+=1
            if al_t(s,t)>=3: c['a3']+=1
        for t in usa[i]: c['u_alta' if al_t(s,t)>=2 else 'u_bassa']+=1
        r.append(f"{n:7} id{i:>2} A iniz {a[0]:2} min {min(a):2} max {max(a):2} | A2+ {100*alto//dur:3}% A3 {100*sum(1 for x in a if x>=3)//dur:3}% | A2+ e filo<=12kn {100*f12//dur:3}% | salite A<2 {c['bassa']} A2+ {c['alta']} di cui A3 {c['a3']} | usato A<2 {c['u_bassa']} A2+ {c['u_alta']}")
    return r
if __name__=='__main__':
    with multiprocessing.Pool(6) as p:
        for r in p.imap(lavora,G):
            print('\n'.join(r),flush=True)
