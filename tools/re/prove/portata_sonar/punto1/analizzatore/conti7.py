import collections,runpy,sys,io,contextlib,os
buf=io.StringIO()
with contextlib.redirect_stdout(buf):
    g=runpy.run_path(os.path.abspath("conti6.py"))
T,S2,R,GR,regola,righe,carica=g["T"],g["S2"],g["R"],g["GR"],g["regola"],g["righe"],g["carica"]
M=[r for r in carica("tracce")]
print("scene senza ordini",len(M))
def griglia(X,nome):
    Y=[r for r in X if r["nr"]>0 and r["V"]]
    print(f"\n## {nome}: {len(Y)} letture con righe; giuste oggi {sum(r['giusta'] for r in Y)}")
    print("oss prese quota | nome | bersaglio fra i nomi | vero | falso | giuste tenute")
    for a in (1,2,3,4):
        for b in (1,2,3):
            if b>a: continue
            for c in (.34,.5,.6,.75):
                D=[(r,regola(r,a,b,c)) for r in Y]; W=[(r,m) for r,m in D if m]
                if not W: continue
                t=sum(any(v["id"]==r["classe"] for v in m) for r,m in W); ve=sum(any(v["id"]==r["classe"] or v["id"] in r["src"] for v in m) for r,m in W)
                k=sum(any(v["id"]==r["classe"] for v in m) for r,m in D if r["giusta"])
                print(a,b,c,"|",len(W),"|",f"{t} {100*t/len(W):.0f}%","|",f"{100*ve/len(W):.0f}%","|",len(W)-ve,"|",k)
griglia([r for r in T if r["gruppo"]=="sub"],"TB-16 sottomarini")
griglia(T,"TB-16 tutti")
print("\n## Le giuste di oggi, TB-16: righe spiegate e quota")
c=collections.Counter()
for r in T:
    if r["giusta"] and r["V"]:
        v=r["V"][0]; c[(r["gruppo"],min(v["oss"],4),"q>=60" if v["oss"]/r["nr"]>=.6 else "q<60")]+=1
for k,n in sorted(c.items()): print(k,n)
print("\n## Il passo con la regola: NONSUB su un sottomarino, nelle varianti")
def passo(r,marg,minimo_sub):
    m=regola(r)
    if not m: return "-"
    best=max(v["oss"] for v in m)
    sub=[v for v in r["V"] if GR(v["id"])=="sub" and v["oss"]>=best-marg and v["oss"]>=minimo_sub]
    if not sub: return "NONSUB"
    return "PROB" if all(GR(v["id"])=="sub" for v in m) else "POSSUB"
for nome,X in (("scene senza ordini",M),("sfera col cavo fuori",S2),("TB-16",T)):
    for g_ in ("sub","guerra","civile"):
        Y=[r for r in X if r["gruppo"]==g_ and r["nr"]>0 and r["V"]]
        a=collections.Counter(passo(r,1,2) for r in Y); b=collections.Counter(passo(r,9,1) for r in Y)
        print(nome,g_,len(Y),"oggi NON",sum(r["passo"]=="NONSUB" for r in Y),"PROB/CERT",sum(r["passo"] in("PROBSUB","CERTSUB") for r in Y),"| P2",dict(a),"| P2 stretta",dict(b))
print("\nI NONSUB su un sottomarino con P2, chi sono:")
for X in (M,T):
    for r in X:
        if r["gruppo"]=="sub" and r["nr"]>0 and r["V"] and passo(r,1,2)=="NONSUB": print(r["scena"],r["seme"],r["t"],r["classe"],r["km"],"nr",r["nr"],"T",r["ntar"],r["V"][0]["id"],r["V"][0]["oss"],r["righe"][:70])
print("\n## Tracce: che cosa dichiara (confidenza) e categoria")
print("chi gruppo classificate NONSUB POSSUB PROB CERT ; con classe: giusta, stesso gruppo; senza classe: tipo a orecchio; errata")
for chi in ("gioc","ia"):
    for g_ in ("sub","guerra","civile","natura"):
        K=[c for c in righe if c[4]==chi and c[9]=="1" and GR(c[8])==g_]
        if not K: continue
        n=collections.Counter(c[11] for c in K); N=[c for c in K if c[10]!="~"]
        print(chi,g_,len(K),n["NONSUB"],n["POSSUB"],n["PROBSUB"],n["CERTSUB"],"; classe",len(N),"giusta",sum(c[10]==c[8] for c in N),"gruppo",sum(GR(c[10])==g_ for c in N),"; tipo",collections.Counter(c[14] for c in K if c[10]=="~").most_common(3),"; errata",sum(c[12]=="1" for c in K))
print("errata per (classe vuota, confidenza, gruppo), gioc+ia, non natura:")
c=collections.Counter()
for x in righe:
    if x[9]=="1" and GR(x[8]) in("sub","guerra","civile"): c[(x[4],GR(x[8]),"~" if x[10]=="~" else "cl",x[11],x[12])]+=1
for k,n in sorted(c.items()): print(k,n)
