#!/usr/bin/env python3
# Tagli che riassunto.py non fa: contatti veri, distanza, traffico, calibrazione, maschera, firma e rapporti; il limite col bootstrap per corsa.
import random, collections, importlib.util, sys
sp = importlib.util.spec_from_file_location("r", __file__.rsplit("/",1)[0] + "/riassunto.py"); R = importlib.util.module_from_spec(sp); sp.loader.exec_module(R)
random.seed(1)
def boot(X, f, n=800):
    # le corse (scena, seme, traffico) sono le unita' indipendenti: si ricampionano quelle, non le letture
    g = collections.defaultdict(list)
    for r in X: g[(r["scena"], r["seme"], r["traffico"])].append(r)
    K = list(g); v = []
    if not K: return (0, 0, 0)
    for _ in range(n):
        a = b = 0
        for k in random.choices(K, k=len(K)):
            a += sum(f(r) for r in g[k]); b += len(g[k])
        v.append(a / max(b, 1))
    v.sort()
    return (sum(f(r) for r in X) / len(X), v[int(.025 * n)], v[int(.975 * n)])
def bp(X, f):
    if not X: return "-"
    a, b, c = boot(X, f)
    return f"{100*a:.0f}% ({100*b:.0f}-{100*c:.0f})"
V = {v: R.senza_baffles(R.carica(v)) for v in ("oggi", "senza_s3", "raggi")}
S = {v: [r for r in V[v] if r["tipo"] == "scena"] for v in V}
print("corse", len({(r['scena'], r['seme'], r['traffico']) for r in S['oggi']}))
print("\n## Tutte le unita' entro 40 km, fascio sul vero, limite col bootstrap per corsa")
print("| Gruppo | taglio | var | letture | nessuna riga | classe giusta su tutte | con righe: giusta | con righe: gruppo giusto | con righe: sbagliata >=50% | con righe: sbagliata >=90% |")
print("|---|---|---|---|---|---|---|---|---|---|")
tagli = [("tutte", lambda r: True), ("con contatto (q>=0)", lambda r: r["q"] >= 0), ("<=10 km", lambda r: r["km"] <= 10), ("<=5 km", lambda r: r["km"] <= 5), ("righe del bersaglio >=1", lambda r: r["ntar"] >= 1), ("righe, nessuna sua", lambda r: r["ntar"] == 0 and r["nr"] > 0)]
for g in ("sub", "guerra", "civile"):
    for nome, t in tagli:
        for v in ("oggi", "senza_s3"):
            X = [r for r in S[v] if r["gruppo"] == g and t(r)]
            Y = [r for r in X if r["nr"] > 0]
            print(f"| {g} | {nome} | {v} | {len(X)} | {sum(r['nr'] == 0 for r in X)} | {bp(X, lambda r: r['giusta'])} | {bp(Y, lambda r: r['giusta'])} | {bp(Y, lambda r: r['giusto_g'])} | {bp(Y, lambda r: not r['giusta'] and r['conf'] >= .5)} | {bp(Y, lambda r: not r['giusta'] and r['conf'] >= .9)} |")
print("\n## Calibrazione: quando dice X%, quante volte ha ragione (scene)")
print("| var | confidenza | letture | classe giusta | gruppo giusto |")
print("|---|---|---|---|---|")
for v in ("oggi", "senza_s3"):
    Y = [r for r in S[v] if r["nr"] > 0]
    for a, b in ((0, .25), (.25, .5), (.5, .75), (.75, .9), (.9, 1.01)):
        X = [r for r in Y if a <= r["conf"] < b]
        print(f"| {v} | {a}-{b} | {len(X)} | {bp(X, lambda r: r['giusta'])} | {bp(X, lambda r: r['giusto_g'])} |")
print("\n## Scene con e senza traffico (stesse scene e semi), oggi e senza S3")
print("| var | traffico | gruppo | letture | nessuna riga | giusta | righe medie | sue |")
print("|---|---|---|---|---|---|---|---|")
for v in ("oggi", "senza_s3"):
    for tr in ("si", "no"):
        for g in ("sub", "guerra", "civile"):
            X = [r for r in S[v] if r["traffico"] == tr and r["gruppo"] == g and r["scena"] in ("SCENE1A", "SCENE2A", "SCENE2B")]
            if X: print(f"| {v} | {tr} | {g} | {len(X)} | {sum(r['nr']==0 for r in X)} | {bp(X, lambda r: r['giusta'])} | {sum(r['nr'] for r in X)/len(X):.1f} | {sum(r['ntar'] for r in X)/len(X):.2f} |")
print("\n## La maschera: righe e classe con la maschera spenta (oggi)")
US = ("los_angeles","spruance","perry","ticonderoga","kidd","sacramento","kitty_hawk","virginia","sturgeon","ohio","knox","burke","arleigh_burke","nimitz","seawolf","permit","typhoon_x")
cat = R.CAT["firme"]
print("| Taglio | letture | righe con | righe senza | righe bersaglio tolte? giusta con | giusta senza |")
print("|---|---|---|---|---|---|")
for nome, F in (("nazionalita' usa", lambda r: cat.get(r["classe"],{}).get("nazionalita") in ("USA","usa","US","Stati Uniti")), ("altre", lambda r: cat.get(r["classe"],{}).get("nazionalita") not in ("USA","usa","US","Stati Uniti"))):
    for t, P in (("scene", S["oggi"]), ("spazzata", [r for r in V["oggi"] if r["tipo"]=="spazzata"])):
        X = [r for r in P if F(r)]
        if X: print(f"| {nome} {t} | {len(X)} | {sum(r['nr'] for r in X)/len(X):.2f} | {sum(r['off_nr'] for r in X)/len(X):.2f} | {sum(r['giusta'] for r in X)}/{len(X)} | {sum(r['off_top']==r['classe'] for r in X)}/{len(X)} |")
print("off_top piu' frequenti", collections.Counter(r["off_top"] for r in S["oggi"]).most_common(5))
print("nazionalita'", collections.Counter(v.get("nazionalita") for v in cat.values()).most_common(8))
print("\n## Rapporti (schermo) e firma (tracker): oggi, scene con righe")
Y = [r for r in S["oggi"] if r["nr"] > 0]
print("firma giusta", R.pc(sum(r["firma"]==r["classe"] for r in Y), len(Y)), "rapporti giusta", R.pc(sum(r["giusta"] for r in Y), len(Y)), "concordano", R.pc(sum(r["firma"]==r["top"] for r in Y), len(Y)))
print("passi", collections.Counter((r["gruppo"], r["passo"]) for r in Y))
print("\n## Pale: bonus e confronto senza pale (oggi, tutte le letture)")
A=[r for r in V["oggi"] if r["nr"]>0]
print("con pale lette", sum(r["pale_l"]>0 for r in A), "giuste le pale", sum(r["pale_l"]==r["pale_v"] for r in A if r["pale_l"]>0))
for t,P in (("scene",[r for r in A if r["tipo"]=="scena" and r["pale_l"]>0]),("spazzata",[r for r in A if r["tipo"]=="spazzata" and r["pale_l"]>0])):
    print(t,len(P),"giusta con pale",sum(r["giusta"] for r in P),"senza",sum(r["np_top"]==r["classe"] for r in P))
print("\n## Il peso lineare dei diesel: la voce in testa e' civile, quante volte con una riga <=30 Hz fra le osservate")
print("| var | voce in testa | letture | conf media | giuste | con una riga <=30 Hz |")
print("|---|---|---|---|---|---|")
for v in ("oggi","senza_s3"):
    Y=[r for r in S[v] if r["nr"]>0]
    for top in ("mercantile","cargo","petroliera","rompighiaccio","nave_ricerca","agi","alfa"):
        X=[r for r in Y if r["top"]==top]
        if X:
            b=sum(any(float(z.split(":")[0])<=30 for z in r["righe"].split(",") if z!="-") for r in X)
            print(f"| {v} | {top} | {len(X)} | {sum(r['conf'] for r in X)/len(X):.2f} | {sum(r['giusta'] for r in X)} | {b} |")
print("\n## Distanza (scene, oggi): per fascia")
print("| Gruppo | km | letture | nessuna riga | giusta | righe medie | sue medie |")
print("|---|---|---|---|---|---|---|")
for g in ("sub","guerra","civile"):
    for a,b in ((0,3),(3,7),(7,15),(15,25),(25,41)):
        X=[r for r in S["oggi"] if r["gruppo"]==g and a<=r["km"]<b]
        if X: print(f"| {g} | {a}-{b} | {len(X)} | {sum(r['nr']==0 for r in X)} | {R.pc(sum(r['giusta'] for r in X),len(X))} | {sum(r['nr'] for r in X)/len(X):.1f} | {sum(r['ntar'] for r in X)/len(X):.2f} |")
print("\n## Baffles esclusi", {v:(len(R.carica(v))-len(V[v])) for v in V})
print("\n## Classi nella spazzata: giusta per classe e velocita' a 1-5 km (oggi), le peggiori e le migliori")
P=[r for r in V["oggi"] if r["tipo"]=="spazzata" and r["km"]<=5]
c=collections.defaultdict(list)
for r in P: c[r["classe"]].append(r)
L=sorted(c.items(),key=lambda kv:sum(r["giusta"] for r in kv[1])/len(kv[1]))
for k,X in L[-12:]: print(k,len(X),"giusta",sum(r["giusta"] for r in X),"nr",round(sum(r["nr"] for r in X)/len(X),1),collections.Counter(r["top"] for r in X if r["nr"]>0).most_common(3))
print("zero giuste:",[k for k,X in L if sum(r["giusta"] for r in X)==0])
