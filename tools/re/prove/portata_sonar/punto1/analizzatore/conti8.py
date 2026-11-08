# Il passo finale (mai NONSUB): su chi esce, e un POSSUB/PROB su una nave ha sotto le righe di un sottomarino vero o solo una coincidenza?
import collections,runpy,io,contextlib,os
buf=io.StringIO()
with contextlib.redirect_stdout(buf):
    g=runpy.run_path(os.path.abspath("conti7.py"))
T,S2,GR,regola,carica=g["T"],g["S2"],g["GR"],g["regola"],g["carica"]
M=[r for r in carica("tracce")]
def passo(r):
    m=regola(r)
    if not m: return "-"
    if all(GR(v["id"])=="sub" for v in m[:2]): return "PROB"
    best=max(v["oss"] for v in m)
    return "POSSUB" if any(GR(v["id"])=="sub" and v["oss"]>=max(2,best-1) for v in r["V"]) else "-"
for nome,X in (("scene senza ordini",M),("sfera col cavo fuori",S2),("TB-16",T)):
    print("\n##",nome)
    for gr in ("sub","guerra","civile"):
        Y=[r for r in X if r["gruppo"]==gr and r["nr"]>0 and r["V"]]; c=collections.Counter(passo(r) for r in Y)
        print(gr,len(Y),dict(c),f"allarme {100*(c['POSSUB']+c['PROB'])/max(1,len(Y)):.0f}%")
    A=[r for r in X if r["gruppo"]!="sub" and r["nr"]>0 and r["V"] and passo(r)!="-"]
    d=collections.Counter(); sc=collections.Counter()
    for r in A:
        vs=[v["id"] for v in r["V"] if GR(v["id"])=="sub"]; srcsub=[s for s in r["src"] if GR(s)=="sub"]
        d["righe di un sottomarino vero nel fascio" if srcsub else "nessun sottomarino fra le sorgenti delle righe"]+=1; sc[r["scena"]]+=1
    print("allarmi su navi:",len(A),dict(d)); print(" per scena:",dict(sc))
print("\n## Variante non applicata: allarme solo se un sottomarino e' fra le voci mostrate a schermo (la prima o la seconda)")
def passo2(r):
    m=regola(r)
    if not m: return "-"
    s=[GR(v["id"])=="sub" for v in m[:2]]
    return "PROB" if all(s) else "POSSUB" if any(s) else "-"
for nome,X in (("scene senza ordini",M),("sfera col cavo fuori",S2),("TB-16",T)):
    for gr in ("sub","guerra","civile"):
        Y=[r for r in X if r["gruppo"]==gr and r["nr"]>0 and r["V"]]; c=collections.Counter(passo2(r) for r in Y)
        print(nome,gr,len(Y),dict(c),f"allarme {100*(c['POSSUB']+c['PROB'])/max(1,len(Y)):.0f}%")
