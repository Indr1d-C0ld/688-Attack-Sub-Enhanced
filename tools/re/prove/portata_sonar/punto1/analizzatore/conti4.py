import importlib.util,collections
sp = importlib.util.spec_from_file_location("c", "conti3.py"); 
src=open("conti3.py").read().split("GR = lambda")[0].split('print("righe intere')[0]
exec(src)
oc = carica("oggi", "oggi_catoggi")
Y=[r for r in oc if ok(r)]
M=[r for r in Y if r["v"][0]["id"]!=r["top"]]
print("diversi",len(M),collections.Counter(r["tipo"] for r in M),"righe",collections.Counter(r["nr"] for r in M).most_common(5))
for r in M[:6]: print(r["classe"],r["pale_l"],r["top"],r["conf"],r["v"][0],r["v"][1],r["righe"][:80])
print("pale_l in diversi",collections.Counter(r["pale_l"] for r in M).most_common(5))
# chi ha fatto le righe: la voce in testa e' una delle sorgenti delle righe?
def sorgenti(r):
    c=collections.Counter()
    for z in r["righe"].split(","):
        q=z.split(":")
        if len(q)>3 and q[2] in "TA" and q[3]: c[q[3]]+=1
    return c
S=[r for r in Y if r["tipo"]=="scena"]
print("\nscene: letture",len(S))
a=sum(r["v"][0]["id"]==r["classe"] for r in S); b=sum(r["v"][0]["id"] in sorgenti(r) for r in S); c=sum(sorgenti(r) and sorgenti(r).most_common(1)[0][0]==r["v"][0]["id"] for r in S)
print("voce == bersaglio",a,"voce fra le sorgenti delle righe",b,"voce == sorgente con piu' righe",c, "senza nessuna riga attribuita",sum(not sorgenti(r) for r in S))
for nm,f in (("<=1",lambda r:r["nr"]<=1),("2-3",lambda r:2<=r["nr"]<=3),("4+",lambda r:r["nr"]>=4),("prese>=3 spiega>=60",lambda r:r["v"][0]["prese"]>=3 and r["v"][0]["oss"]/r["nr"]>=.6)):
    X=[r for r in S if f(r)]
    print(nm,len(X),"sorgente fra le righe",sum(r["v"][0]["id"] in sorgenti(r) for r in X),"bersaglio",sum(r["v"][0]["id"]==r["classe"] for r in X))
