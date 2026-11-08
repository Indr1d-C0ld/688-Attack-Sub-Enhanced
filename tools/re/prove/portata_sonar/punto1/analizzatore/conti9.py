#!/usr/bin/env python3
# Il binario finale (melchior688_m4, passo_da_voci vera) accanto al modello Python della variante: sugli stessi dati nuovi, e sui dati vecchi di conti8.
import collections, glob, importlib.util, os, re
sp = importlib.util.spec_from_file_location("r", os.path.dirname(os.path.abspath(__file__)) + "/riassunto.py"); R = importlib.util.module_from_spec(sp); sp.loader.exec_module(R)
BASE = R.BASE; COL = R.COL + ["voci", "tolte", "voci_lati", "gioc", "sensore", "v_g", "cavo", "scritta"]; GR = R.gruppo
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
            if len(c) not in (len(COL), len(COL) - 1): continue
            r = dict(zip(COL, c))
            for k in ("km", "rel", "v_g", "cavo"): r[k] = float(r[k])
            r["nr"] = int(r["nr"]); r["ti"] = float(r["t"]); r["danno"] = danno; r["V"] = voci(r["voci"])
            r["src"] = collections.Counter()
            for z in r["righe"].split(","):
                q = z.split(":")
                if len(q) > 3 and q[2] in "TA" and q[3]: r["src"][q[3]] += 1
            r["key"] = (r["scena"], r["seme"], r["traffico"], r["t"], r["id"])
            out.append(r)
    return out
def regola(r):
    ok = [v for v in r["V"] if v["oss"] >= 3 and v["prese"] >= 2 and v["oss"] / max(r["nr"], 1) >= .6]
    ok.sort(key=lambda v: (-v["oss"], -v["conf"]))
    return ok[:2]
def modello(r):
    m = regola(r)
    if not m: return "-"
    s = [GR(v["id"]) == "sub" for v in m]
    return "PROBSUB" if all(s) else "POSSUB" if any(s) else "-"
def tre(sc, tb, sf):
    S, T0 = carica(sc), carica(tb)
    T0 = [r for r in T0 if r["gioc"] == "los_angeles" and r["cavo"] > 0 and r["ti"] < r["danno"]]
    ks = {r["key"]: r for r in carica(sf)}
    T = [r for r in T0 if r["key"] in ks]; S2 = [ks[r["key"]] for r in T]
    return (("scene", S), ("sfera in moto", S2), ("TB-16", T))
def conta(X, gr, f):
    Y = [r for r in X if r["gruppo"] == gr and r["nr"] > 0 and r["V"]]
    c = collections.Counter(f(r) for r in Y)
    return len(Y), c
NUOVI = tre("fin_scene", "fin_tb16", "fin_sfera"); VECCHI = dict(tre("tracce", "cavo_tb16", "cavo_sfera"))
print("| Colonna | Bersagli | Letture con righe | Binario: POS / PROB | Binario: allarme | Modello, stessi dati | Modello, dati vecchi | Letture con righe, dati vecchi |")
print("|---|---|---|---|---|---|---|---|")
tot = dis = 0
for nome, X in NUOVI:
    for gr in ("sub", "guerra", "civile"):
        n, c = conta(X, gr, lambda r: r["passo"]); _, m = conta(X, gr, modello); nv, v = conta(VECCHI[nome], gr, modello)
        a = lambda q, d: f"{100 * (q['POSSUB'] + q['PROBSUB']) / max(1, d):.1f}%"
        print(f"| {nome} | {gr} | {n} | {c['POSSUB']} / {c['PROBSUB']} | {a(c, n)} | {a(m, n)} | {a(v, nv)} | {nv} |")
print("\n## Binario contro modello, lettura per lettura")
for nome, X in NUOVI:
    Y = [r for r in X if r["nr"] > 0 and r["V"]]
    d = [r for r in Y if r["passo"] != modello(r)]
    nn = sum(r["passo"] == "NONSUB" for r in X); cc = sum(r["passo"] == "CERTSUB" for r in X)
    print(nome, "letture", len(X), "con righe", len(Y), "passo diverso dal modello", len(d), "NONSUB", nn, "CERTSUB", cc)
    print("  nome a schermo contro regola diversi:", sum(("RIGHE O " in r.get("scritta", "") or "NESSUN" not in r.get("scritta", "") and bool(r.get("scritta"))) != bool(regola(r)) for r in Y))
    for r in d[:6]: print("  ", r["scena"], r["seme"], r["t"], r["classe"], r["passo"], modello(r), r["scritta"], [(v["id"], v["oss"], v["prese"]) for v in r["V"][:4]], r["nr"])
print("\n## Allarmi sulle navi nel binario: c'e' un sottomarino fra le sorgenti delle righe?")
for nome, X in NUOVI:
    A = [r for r in X if r["gruppo"] in ("guerra", "civile") and r["nr"] > 0 and r["V"] and r["passo"] in ("POSSUB", "PROBSUB")]
    print(nome, "allarmi", len(A), "con righe di un sottomarino vero", sum(any(GR(s) == "sub" for s in r["src"]) for r in A), "per scena", dict(collections.Counter(r["scena"] for r in A)))
print("\n## Sui sottomarini, chi da' l'allarme (classe, passo)")
for nome, X in NUOVI:
    print(nome, sorted(collections.Counter((r["classe"], r["passo"]) for r in X if r["gruppo"] == "sub" and r["nr"] > 0 and r["passo"] in ("POSSUB", "PROBSUB")).items()))
print("\n## Il nome a schermo nel binario (scritta), P1")
for nome, X in NUOVI:
    Y = [r for r in X if r["nr"] > 0 and r["V"]]; W = [(r, regola(r)) for r in Y if regola(r)]
    t = sum(any(v["id"] == r["classe"] for v in m) for r, m in W); ve = sum(any(v["id"] == r["classe"] or v["id"] in r["src"] for v in m) for r, m in W)
    print(nome, "con righe", len(Y), "con nome", len(W), "bersaglio fra i nomi", t, "nome vero", f"{100 * ve / max(1, len(W)):.0f}%")
