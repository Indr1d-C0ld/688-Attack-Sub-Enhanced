#!/usr/bin/env python3
# Sulle righe rigiocate (esiti/rig): quale catalogo fa il 97%, il tetto al peso delle righe, le soglie dello schermo con il loro prima e dopo.
import collections, importlib.util, json, random, sys
sp = importlib.util.spec_from_file_location("r", __file__.rsplit("/",1)[0] + "/riassunto.py"); R = importlib.util.module_from_spec(sp); sp.loader.exec_module(R)
BASE = R.BASE
COL = R.COL
def carica(v, rig):
    rows = []
    with open(f"{BASE}/rig/{v}.tsv") as a, open(f"{BASE}/rig/{rig}.txt") as b:
        for l, m in zip(a, b):
            c = l.rstrip("\n").split("\t")
            if len(c) != len(COL): continue
            r = dict(zip(COL, c))
            for k in ("km", "rel", "conf", "v_kn"): r[k] = float(r[k])
            for k in ("nr", "ntar", "nalt", "nvuo", "conc"): r[k] = int(r[k])
            r["tipo"] = "scena" if r["prova"] == "scena" else "spazzata"
            r["baffles"] = abs(r["rel"]) > R.BAFFLES
            p = m.rstrip("\n").split("\t|")
            r["n_rig"] = int(p[0].split("\t")[0])
            def voci(s):
                o = []
                for x in s.split("\t"):
                    q = x.split(":")
                    if len(q) == 6: o.append(dict(id=q[0], conf=float(q[1]), oss=int(q[2]), fisse=int(q[3]), piu=int(q[4]), prese=int(q[5])))
                return o
            r["v"] = voci(p[0]); r["t"] = voci(p[1]) if len(p) > 1 else []
            rows.append(r)
    return [r for r in rows if not r["baffles"]]
def ok(r): return r["nr"] > 0 and r["n_rig"] == r["nr"]
oc = carica("oggi", "oggi_catoggi"); op = carica("oggi", "oggi_catprima"); sc = carica("senza_s3", "s3_catoggi"); sp_ = carica("senza_s3", "s3_catprima"); rg = carica("raggi", "raggi_catoggi")
Y = [r for r in oc if ok(r)]
print("righe intere rigiocabili", len(Y), "su", sum(r["nr"] > 0 for r in oc), "; rigioco == campagna:", sum(r["v"][0]["id"] == r["top"] for r in Y), "conf entro 0,01:", sum(abs(r["v"][0]["conf"] - r["conf"]) < .01 for r in Y))
print("\n## Da dove viene la sicurezza: stesse righe, due cataloghi (scene)")
print("| Righe di | Catalogo | Letture | Conf media | Sbagliata >=90% | Giusta |")
print("|---|---|---|---|---|---|")
for nome, X, cat in (("oggi", oc, "oggi"), ("oggi", op, "prima di S3"), ("senza S3", sc, "oggi"), ("senza S3", sp_, "prima di S3")):
    Z = [r for r in X if r["tipo"] == "scena" and ok(r)]
    g = lambda r: r["v"][0]["id"] == r["classe"]
    print(f"| {nome} | {cat} | {len(Z)} | {sum(r['v'][0]['conf'] for r in Z)/len(Z):.2f} | {R.pc(sum(not g(r) and r['v'][0]['conf']>=.9 for r in Z), len(Z))} | {R.pc(sum(g(r) for r in Z), len(Z))} |")
print("\n## Il tetto a 1 sul peso di una riga (diesel +12 = 4): stesse righe, oggi")
print("| Taglio | Letture | Conf media prima | dopo | >=90% e sbagliata prima | dopo | Giusta prima | dopo |")
print("|---|---|---|---|---|---|---|---|")
for nome, Z in (("scene", [r for r in oc if r["tipo"]=="scena" and ok(r)]), ("spazzata", [r for r in oc if r["tipo"]=="spazzata" and ok(r)])):
    a = lambda r: r["v"][0]; b = lambda r: r["t"][0]
    print(f"| {nome} | {len(Z)} | {sum(a(r)['conf'] for r in Z)/len(Z):.2f} | {sum(b(r)['conf'] for r in Z)/len(Z):.2f} | {R.pc(sum(a(r)['id']!=r['classe'] and a(r)['conf']>=.9 for r in Z),len(Z))} | {R.pc(sum(b(r)['id']!=r['classe'] and b(r)['conf']>=.9 for r in Z),len(Z))} | {R.pc(sum(a(r)['id']==r['classe'] for r in Z),len(Z))} | {R.pc(sum(b(r)['id']==r['classe'] for r in Z),len(Z))} |")
GR = lambda i: R.gruppo(i)
SOGLIE = {
 "oggi: sempre un nome": lambda r, t: True,
 ">=2 righe osservate spiegate": lambda r, t: t["oss"] >= 2,
 ">=3 righe della voce": lambda r, t: t["prese"] >= 3,
 ">=3 righe della voce, spiega >=60%": lambda r, t: t["prese"] >= 3 and t["oss"] / max(r["nr"], 1) >= .6,
 ">=3, spiega >=60%, seconda altro gruppo sotto": lambda r, t: t["prese"] >= 3 and t["oss"] / max(r["nr"], 1) >= .6 and not any(GR(u["id"]) != GR(t["id"]) and u["prese"] >= t["prese"] for u in r["v"][1:]),
 ">=4 righe della voce, spiega >=60%": lambda r, t: t["prese"] >= 4 and t["oss"] / max(r["nr"], 1) >= .6,
 ">=4, spiega >=75%": lambda r, t: t["prese"] >= 4 and t["oss"] / max(r["nr"], 1) >= .75,
}
def tabella(titolo, Z):
    print(f"\n## {titolo} ({len(Z)} letture con righe)")
    print("| Soglia | Dà un nome | Classe giusta quando lo dà | Gruppo giusto | Nome sbagliato su tutte |")
    print("|---|---|---|---|---|")
    for n, f in SOGLIE.items():
        W = [r for r in Z if f(r, r["v"][0])]
        g = sum(r["v"][0]["id"] == r["classe"] for r in W); h = sum(GR(r["v"][0]["id"]) == GR(r["classe"]) for r in W)
        print(f"| {n} | {R.pc(len(W), len(Z))} | {R.pc(g, len(W))} | {R.pc(h, len(W))} | {R.pc(len(W) - g, len(Z))} |")
S = [r for r in oc if r["tipo"]=="scena" and ok(r)]; P = [r for r in oc if r["tipo"]=="spazzata" and ok(r)]
tabella("Scene, oggi", S)
for g in ("sub", "guerra", "civile"):
    tabella(f"Scene, oggi, {g}", [r for r in S if r["gruppo"] == g])
tabella("Scene, righe del bersaglio presenti", [r for r in S if r["ntar"] >= 1])
tabella("Spazzata (un bersaglio solo), oggi", P)
tabella("Spazzata, LOFAR ai raggi", [r for r in rg if r["tipo"]=="spazzata" and ok(r)])
tabella("Scene, LOFAR ai raggi", [r for r in rg if r["tipo"]=="scena" and ok(r)])
json.dump([{k: r[k] for k in ("scena","seme","traffico","t","classe","gruppo","km","nr","ntar","v")} for r in S], open(f"{BASE}/rig/scene.json","w"))
