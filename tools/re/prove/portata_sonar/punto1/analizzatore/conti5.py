#!/usr/bin/env python3
# La regola proposta per lo schermo sulle letture a due decimali (esiti/m2 e gemelle); la maschera per nazionalita'; la TB-16; le frange; i lati.
import collections, glob, importlib.util, random, sys, tomllib
sp = importlib.util.spec_from_file_location("r", __file__.rsplit("/",1)[0] + "/riassunto.py"); R = importlib.util.module_from_spec(sp); sp.loader.exec_module(R)
BASE = R.BASE; COL = R.COL + ["voci", "tolte", "voci_lati"]
GR = R.gruppo
def voci(s):
    o = []
    if s == "-": return o
    for x in s.split(";"):
        q = x.split(":")
        o.append(dict(id=q[0], conf=float(q[1]), oss=int(q[2]), fisse=int(q[3]), piu=int(q[4]), prese=int(q[5])))
    return o
def carica(d):
    out = []
    for f in glob.glob(f"{BASE}/{d}/*.tsv"):
        for l in open(f):
            c = l.rstrip("\n").split("\t")
            if len(c) != len(COL): continue
            r = dict(zip(COL, c))
            for k in ("km", "rel", "conf", "v_kn"): r[k] = float(r[k])
            for k in ("nr", "ntar", "nalt", "nvuo", "conc", "tolte", "e_n", "d_n"): r[k] = int(r[k])
            if abs(r["rel"]) > R.BAFFLES: continue
            r["V"] = voci(r["voci"]); r["L"] = voci(r["voci_lati"])
            r["src"] = collections.Counter()
            for z in r["righe"].split(","):
                q = z.split(":")
                if len(q) > 3 and q[2] in "TA" and q[3]: r["src"][q[3]] += 1
            out.append(r)
    return out
def regola(V, n, minimo=3, quota=.6):
    # le voci che spiegano almeno `minimo` righe osservate e la quota delle osservate; in mostra le due che ne spiegano di piu'
    ok = [v for v in V if v["oss"] >= minimo and v["prese"] >= 2 and v["oss"] / max(n, 1) >= quota]
    ok.sort(key=lambda v: (-v["oss"], -v["conf"]))
    return ok[:2]
def bilancio(titolo, X, sel=lambda r: r["V"], nn=lambda r: r["nr"]):
    print(f"\n### {titolo}")
    print("| Gruppo | Letture con righe | Oggi: nome giusto | Oggi: nome che non c'è nelle righe né è il bersaglio | Con la regola: dà un nome | ...il bersaglio fra i nomi | ...nome vero nel fascio | ...nome falso | Silenzio su righe del bersaglio |")
    print("|---|---|---|---|---|---|---|---|---|")
    for g in ("sub", "guerra", "civile", "tutti"):
        Y = [r for r in X if (g == "tutti" or r["gruppo"] == g) and r["nr"] > 0 and r["V"]]
        if not Y: continue
        og = sum(r["V"][0]["id"] == r["classe"] for r in Y)
        of = sum(r["V"][0]["id"] != r["classe"] and r["V"][0]["id"] not in r["src"] for r in Y)
        D = [(r, regola(sel(r), nn(r))) for r in Y]
        W = [(r, m) for r, m in D if m]
        b = sum(any(v["id"] == r["classe"] for v in m) for r, m in W)
        ve = sum(any(v["id"] == r["classe"] or v["id"] in r["src"] for v in m) for r, m in W)
        si = sum(not m and r["ntar"] >= 1 for r, m in D)
        print(f"| {g} | {len(Y)} | {R.pc(og, len(Y))} | {R.pc(of, len(Y))} | {R.pc(len(W), len(Y))} | {R.pc(b, len(W))} | {R.pc(ve, len(W))} | {R.pc(len(W) - ve, len(W))} | {si} |")
M = carica("m2"); F = carica("m2_fr"); T = carica("tb16"); G = carica("raggi2")
print("# letture", len(M), len(F), len(T), len(G))
bilancio("Oggi, sfera, 5 semi", M)
bilancio("Oggi, righe del bersaglio presenti", [r for r in M if r["ntar"] >= 1])
bilancio("Senza frange", F)
bilancio("TB-16 forzata (senza cavo filato)", T)
bilancio("LOFAR ai raggi", G)
print("\n### Che cosa fa la voce in testa, oggi: bersaglio, una sorgente vera delle righe, nulla")
for nm, X in (("sfera", M), ("senza frange", F), ("tb16", T), ("raggi", G)):
    Y = [r for r in X if r["nr"] > 0 and r["V"]]
    a = sum(r["V"][0]["id"] == r["classe"] for r in Y); b = sum(r["V"][0]["id"] != r["classe"] and r["V"][0]["id"] in r["src"] for r in Y)
    print(nm, len(Y), "bersaglio", R.pc(a, len(Y)), "altra sorgente vera", R.pc(b, len(Y)), "righe", sum(r["nr"] for r in Y), "T", sum(r["ntar"] for r in Y), "A", sum(r["nalt"] for r in Y), "v", sum(r["nvuo"] for r in Y), "nessuna riga", sum(r["nr"] == 0 for r in X), "su", len(X))
print("\n### Sub/guerra/civile: classe giusta su TUTTE le letture, per sensore")
print("| Gruppo | sfera | senza frange | tb16 | raggi |")
for g in ("sub", "guerra", "civile"):
    print(f"| {g} | " + " | ".join(R.pc(sum(r["top"] == r["classe"] and r["nr"] > 0 for r in X if r["gruppo"] == g), sum(r["gruppo"] == g for r in X)) for X in (M, F, T, G)) + " |")
print("\n### TB-16: righe per lettura e del bersaglio")
for nm, X in (("sfera", M), ("tb16", T)):
    for g in ("sub", "guerra", "civile"):
        Y = [r for r in X if r["gruppo"] == g]
        if Y: print(nm, g, len(Y), "righe", round(sum(r["nr"] for r in Y) / len(Y), 1), "sue", round(sum(r["ntar"] for r in Y) / len(Y), 2), "con sue righe", sum(r["ntar"] >= 1 for r in Y))
print("\n### Lati (colore per rilevamento): righe tolte e prima voce senza di esse")
Y = [r for r in M if r["nr"] > 0 and r["V"]]
print("righe", sum(r["nr"] for r in Y), "tolte", sum(r["tolte"] for r in Y), "bersaglio prima", sum(r["V"][0]["id"] == r["classe"] for r in Y), "dopo", sum(bool(r["L"]) and r["L"][0]["id"] == r["classe"] for r in Y))
bilancio("Oggi, senza le righe dei lati", M, sel=lambda r: r["L"], nn=lambda r: r["nr"] - r["tolte"])
# la maschera: le righe della voce che cadono sulle proprie, entro 0,2 Hz
d = R.CAT["firme"]
print("\n### La maschera: righe fisse della voce entro 0,2 Hz dalle proprie (catalogo, senza scarti d'individuo)")
for mio in ("los_angeles", "alfa"):
    prop = [r[0] for r in d[mio]["righe"]]
    print(mio, "proprie:", prop)
    for naz in ("usa", "urss", "civile", "uk"):
        c = [k for k, v in d.items() if v.get("nazionalita") == naz and v.get("righe") and k != mio and v.get("tipo_unita") not in ("siluro", "esca")]
        tot = sum(len(d[k]["righe"]) for k in c); via = sum(any(abs(r[0] - h) <= .2 for h in prop) for k in c for r in d[k]["righe"])
        print(" ", naz, len(c), "voci, righe", tot, "tolte", via, f"{100*via/max(tot,1):.0f}%")
A = [r for r in R.carica("oggi") if r["tipo"] == "spazzata"]
print("\n### La maschera in prova: stessi bersagli, giocatore Los Angeles o Alfa (spazzata, 1-5 km, sfera o Skat)")
for naz in ("usa", "urss"):
    for p in ("spazzata_los_angeles", "spazzata_alfa"):
        Y = [r for r in A if r["prova"] == p and d.get(r["classe"], {}).get("nazionalita") == naz and r["km"] <= 5 and r["classe"] not in ("los_angeles", "alfa")]
        print(naz, p, len(Y), "righe", round(sum(r["nr"] for r in Y) / max(len(Y), 1), 2), "sue", round(sum(r["ntar"] for r in Y) / max(len(Y), 1), 2), "giusta", sum(r["giusta"] for r in Y), "off: righe", round(sum(r["off_nr"] for r in Y) / max(len(Y), 1), 1))
# il passo del tracker: oggi dalla somma delle confidenze (confronta_firma); con la regola: NONSUB solo se nessuna voce-sottomarino fra le prime sei spiega quanto la prima meno una riga
print("\n### Il passo del tracker, oggi e con la regola (letture con righe; prime sei voci per confidenza)")
print("| Gruppo | Letture | Oggi NONSUB | Oggi PROB/CERT | Regola: nessun passo | Regola: NONSUB | Regola: POSSUB | Regola: PROB |")
print("|---|---|---|---|---|---|---|---|")
def passo2(r):
    m = regola(r["V"], r["nr"])
    if not m: return "-"
    best = max(v["oss"] for v in m)
    sub = [v for v in r["V"] if GR(v["id"]) == "sub" and v["oss"] >= best - 1 and v["oss"] >= 2]
    if not sub: return "NONSUB"
    return "PROB" if all(GR(v["id"]) == "sub" for v in m) else "POSSUB"
for g in ("sub", "guerra", "civile"):
    Y = [r for r in M if r["gruppo"] == g and r["nr"] > 0 and r["V"]]
    c = collections.Counter(passo2(r) for r in Y)
    print(f"| {g} | {len(Y)} | {sum(r['passo'] == 'NONSUB' for r in Y)} | {sum(r['passo'] in ('PROBSUB', 'CERTSUB') for r in Y)} | {c['-']} | {c['NONSUB']} | {c['POSSUB']} | {c['PROB']} |")
