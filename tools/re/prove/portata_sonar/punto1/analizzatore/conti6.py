#!/usr/bin/env python3
# TB-16 col cavo filato davvero contro la sfera, stessi istanti della stessa partita; le soglie della P1 sui nomi giusti; le tracce di sonarista e IA.
import collections, glob, importlib.util, os, re, sys
sp = importlib.util.spec_from_file_location("r", __file__.rsplit("/",1)[0] + "/riassunto.py"); R = importlib.util.module_from_spec(sp); sp.loader.exec_module(R)
BASE = R.BASE; COL = R.COL + ["voci", "tolte", "voci_lati", "gioc", "sensore", "v_g", "cavo"]
GR = R.gruppo
def voci(s):
    if s == "-": return []
    o = []
    for x in s.split(";"):
        q = x.split(":")
        o.append(dict(id=q[0], conf=float(q[1]), oss=int(q[2]), fisse=int(q[3]), piu=int(q[4]), prese=int(q[5])))
    return o
def carica(d):
    out = []
    for f in glob.glob(f"{BASE}/{d}/*.tsv"):
        danno = 1e9
        m = f[:-4] + ".msg"
        if os.path.exists(m):
            for l in open(m):
                g = re.match(r"\s*t=\s*([\d.]+) s\s+messaggio\s+(39[78]) ", l)
                if g: danno = min(danno, float(g.group(1)))
        for l in open(f):
            c = l.rstrip("\n").split("\t")
            if len(c) != len(COL): continue
            r = dict(zip(COL, c))
            for k in ("km", "rel", "conf", "v_kn", "v_g", "cavo"): r[k] = float(r[k])
            for k in ("nr", "ntar", "nalt", "nvuo", "conc", "tolte"): r[k] = int(r[k])
            r["ti"] = float(r["t"]); r["danno"] = danno
            r["V"] = voci(r["voci"])
            r["src"] = collections.Counter()
            for z in r["righe"].split(","):
                q = z.split(":")
                if len(q) > 3 and q[2] in "TA" and q[3]: r["src"][q[3]] += 1
            r["key"] = (r["scena"], r["seme"], r["traffico"], r["t"], r["id"])
            r["giusta"] = r["nr"] > 0 and r["top"] == r["classe"]
            out.append(r)
    return out
def regola(r, minimo=3, prese=2, quota=.6):
    ok = [v for v in r["V"] if v["oss"] >= minimo and v["prese"] >= prese and v["oss"] / max(r["nr"], 1) >= quota]
    ok.sort(key=lambda v: (-v["oss"], -v["conf"]))
    return ok[:2]
T = carica("cavo_tb16"); S = carica("cavo_sfera")
print("letture grezze", len(T), len(S), "giocatori", collections.Counter(r["gioc"] for r in T))
T = [r for r in T if r["gioc"] == "los_angeles" and r["cavo"] > 0 and r["ti"] < r["danno"]]
ks = {r["key"]: r for r in S}; T = [r for r in T if r["key"] in ks]; S2 = [ks[r["key"]] for r in T]
print("con schiera buona", len(T), "cavo mediano", sorted(r["cavo"] for r in T)[len(T) // 2] if T else 0, "velocita' mediana", sorted(r["v_g"] for r in T)[len(T) // 2] if T else 0)
print("per scena", collections.Counter(r["scena"] for r in T))
SETT = (("settore comune 25-145 gradi", lambda r: 25 <= abs(r["rel"]) <= 145), ("a poppa, >145: la sfera e' sorda", lambda r: abs(r["rel"]) > 145), ("a prua, <25: la schiera e' cieca", lambda r: abs(r["rel"]) < 25))
for nome, f in SETT:
    print(f"\n### {nome}")
    print("| Gruppo | Sensore | Letture | Nessuna riga | Righe medie | Sue medie | Con righe sue | Classe giusta, su tutte | Giusta con righe sue | Gruppo giusto, con righe | Sbagliata >=90%, con righe |")
    print("|---|---|---|---|---|---|---|---|---|---|---|")
    for g in ("sub", "guerra", "civile"):
        for nm, X in (("sfera", S2), ("TB-16", T)):
            Y = [r for r in X if r["gruppo"] == g and f(r)]
            if not Y: continue
            Z = [r for r in Y if r["nr"] > 0]; W = [r for r in Y if r["ntar"] >= 1]
            print(f"| {g} | {nm} | {len(Y)} | {len(Y)-len(Z)} | {sum(r['nr'] for r in Y)/len(Y):.1f} | {sum(r['ntar'] for r in Y)/len(Y):.2f} | {len(W)} | {R.pc(sum(r['giusta'] for r in Y), len(Y))} | {R.pc(sum(r['giusta'] for r in W), len(W))} | {R.pc(sum(GR(r['top']) == g for r in Z), len(Z))} | {R.pc(sum(not r['giusta'] and r['conf'] >= .9 for r in Z), len(Z))} |")
print("\n### Origine delle righe")
for nm, X in (("sfera", S2), ("TB-16", T)):
    print(nm, "righe", sum(r["nr"] for r in X), "sue", sum(r["ntar"] for r in X), "altre", sum(r["nalt"] for r in X), "senza origine", sum(r["nvuo"] for r in X))
print("\n### Le soglie della P1 sui nomi giusti")
print("| Sensore | Gruppo | Letture con righe | Oggi giuste | Soglia | Da' un nome | Bersaglio fra i nomi | Nome vero nel fascio | Giuste di oggi perse |")
print("|---|---|---|---|---|---|---|---|---|")
for nm, X in (("sfera", S2), ("TB-16", T), ("sfera, senza cavo", None)):
    if X is None: continue
    for g in ("sub", "guerra", "civile", "tutti"):
        Y = [r for r in X if (g == "tutti" or r["gruppo"] == g) and r["nr"] > 0 and r["V"]]
        if not Y: continue
        for sn, (a, b, c) in (("3/2/60%", (3, 2, .6)), ("2/2/60%", (2, 2, .6)), ("2/1/50%", (2, 1, .5))):
            D = [(r, regola(r, a, b, c)) for r in Y]
            W = [(r, m) for r, m in D if m]
            t = sum(any(v["id"] == r["classe"] for v in m) for r, m in W)
            ve = sum(any(v["id"] == r["classe"] or v["id"] in r["src"] for v in m) for r, m in W)
            ok = [(r, m) for r, m in D if r["giusta"]]
            pe = sum(not any(v["id"] == r["classe"] for v in m) for r, m in ok)
            print(f"| {nm} | {g} | {len(Y)} | {len(ok)} | {sn} | {R.pc(len(W), len(Y))} | {R.pc(t, len(W))} | {R.pc(ve, len(W))} | {pe}/{len(ok)} |")
print("\n### Il passo del tracker col cavo")
print("| Sensore | Gruppo | Letture con righe | NONSUB | PROB/CERT |")
for nm, X in (("sfera", S2), ("TB-16", T)):
    for g in ("sub", "guerra", "civile"):
        Y = [r for r in X if r["gruppo"] == g and r["nr"] > 0]
        print(f"| {nm} | {g} | {len(Y)} | {sum(r['passo']=='NONSUB' for r in Y)} | {sum(r['passo'] in ('PROBSUB','CERTSUB') for r in Y)} |")
# le tracce: l'ultima lettura di ogni traccia in ogni corsa
print("\n### Tracce dei sonaristi (scene senza ordini): l'ultimo istante di ogni traccia")
righe = []
for f in glob.glob(f"{BASE}/tracce/*.tracce"):
    ult = {}
    for l in open(f):
        c = l.rstrip("\n").split("\t")
        if len(c) != 16: continue
        ult[(c[4], c[5], c[7], c[9] == "1")] = c
    # l'ultima classificata di ogni coppia osservatore, sorgente
    fin = {}
    for l in open(f):
        c = l.rstrip("\n").split("\t")
        if len(c) == 16: fin[(c[4], c[5], c[7])] = c
    righe.extend(fin.values())
print("tracce", len(righe), "stati", collections.Counter(c[9] for c in righe))
print("| Chi | Bersaglio | Tracce | Classificate | Con una classe | Classe giusta | Natura giusta (sub o no) | Dichiara NONSUB | Errata (verita' del gioco) |")
print("|---|---|---|---|---|---|---|---|---|")
for chi in ("gioc", "ia"):
    for g in ("sub", "guerra", "civile", "natura", "umano"):
        X = [c for c in righe if c[4] == chi and GR(c[8]) == g]
        if not X: continue
        K = [c for c in X if c[9] != "0" and (c[10] != "~" or c[11] != "POSSUB" or c[12] == "1" or c[9] == "1")]
        K = [c for c in X if c[9] == "1"]
        N = [c for c in K if c[10] != "~"]
        nat = sum((c[11] != "NONSUB") == (g == "sub") for c in K)
        print(f"| {chi} | {g} | {len(X)} | {len(K)} | {len(N)} | {R.pc(sum(c[10] == c[8] for c in N), len(N))} | {R.pc(nat, len(K))} | {sum(c[11] == 'NONSUB' for c in K)} | {R.pc(sum(c[12] == '1' for c in K), len(K))} |")
print("\nLe classi dette piu' spesso sbagliate (ia e gioc):")
E=[c for c in righe if c[9]=="1" and c[10]!="~" and c[10]!=c[8]]
print(collections.Counter((c[8],c[10]) for c in E).most_common(14))
print("\nCon quale esperienza non e' nel dato; distanza mediana delle classificate:", sorted(float(c[15]) for c in righe if c[9]=="1" and float(c[15])>=0)[len([c for c in righe if c[9]=="1" and float(c[15])>=0])//2] if righe else 0)
print("\nPer distanza: giocatore e IA, sub e navi, natura giusta")
for chi in ("gioc","ia"):
    for a,b in ((0,5),(5,15),(15,40),(40,999)):
        K=[c for c in righe if c[4]==chi and c[9]=="1" and a<=float(c[15])<b and GR(c[8]) in("sub","guerra","civile")]
        if K: print(chi,a,b,len(K),"natura",R.pc(sum((c[11]!="NONSUB")==(GR(c[8])=="sub") for c in K),len(K)),"classe su quelle con classe",R.pc(sum(c[10]==c[8] for c in K if c[10]!="~"),sum(c[10]!="~" for c in K)))
