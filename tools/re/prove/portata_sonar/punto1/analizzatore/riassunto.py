#!/usr/bin/env python3
# I conti della campagna dell'analizzatore: esiti/{oggi,senza_s3,raggi}/*.tsv. Gli intervalli sono di Wilson al 95%, sulle letture:
# le letture di una stessa unita' in istanti e semi diversi non sono indipendenti, quindi il limite vero e' un po' piu' largo.
import csv, glob, math, os, sys, tomllib, collections

BASE = os.path.dirname(os.path.abspath(__file__)) + "/esiti"
COL = open(BASE + "/intestazione.tsv").read().rstrip("\n").split("\t")
ROOT = os.path.abspath(BASE + "/../../../../../../..")
CAT = tomllib.load(open(ROOT + "/data/motore/acustica.toml", "rb"))
BAFFLES = 145.0   # bqq5_sfera: 35 gradi di semiapertura a poppa

def gruppo(i):
    if i in CAT["civili"]: return "civile"
    f = CAT["firme"].get(i)
    if f:
        t = f.get("tipo_unita")
        if t in ("ssn", "ssgn", "ssbn", "ssk"): return "sub"
        if t in ("siluro", "esca"): return "arma"
        if t == "biologico": return "natura"
        if t in ("mercantile", "petroliera", "peschereccio", "generica"): return "civile"
        return "guerra"
    s = CAT["sorgenti"].get(i)
    if s: return "umano" if s.get("categoria") in ("civile", "industriale") else "natura"
    return "?"

def carica(var):
    out = []
    for f in glob.glob(f"{BASE}/{var}/*.tsv"):
        for l in open(f):
            c = l.rstrip("\n").split("\t")
            if len(c) != len(COL): continue
            r = dict(zip(COL, c))
            for k in ("km", "rel", "v_kn", "conf", "conf2", "fconf", "c_conf", "d_conf", "e_conf", "np_conf", "off_conf", "mg_max", "q"):
                r[k] = float(r[k])
            for k in ("nr", "ntar", "nalt", "npro", "nvuo", "conc", "fconc", "c_n", "d_n", "e_n", "mg_n", "mg_tot", "off_nr", "pale_v", "pale_l", "c_cl", "d_cl", "e_cl"):
                r[k] = int(r[k])
            r["baffles"] = abs(r["rel"]) > BAFFLES
            r["tipo"] = "scena" if r["prova"] == "scena" else "spazzata"
            r["gtop"] = gruppo(r["top"]) if r["nr"] > 0 else "-"
            r["giusta"] = r["nr"] > 0 and r["top"] == r["classe"]
            r["giusto_g"] = r["nr"] > 0 and r["gtop"] == r["gruppo"]
            vicini = [x.split(":")[0] for x in r["vicini"].split(",")] if r["vicini"] != "-" else []
            r["nel_fascio"] = r["nr"] > 0 and (r["top"] == r["classe"] or r["top"] in vicini)
            out.append(r)
    return out

def wilson(k, n):
    if n == 0: return (0.0, 0.0)
    z = 1.96; p = k / n
    d = 1 + z * z / n
    c = p + z * z / (2 * n)
    m = z * math.sqrt(p * (1 - p) / n + z * z / (4 * n * n))
    return ((c - m) / d, (c + m) / d)

def pc(k, n):
    if n == 0: return "-"
    a, b = wilson(k, n)
    return f"{k}/{n} = {100 * k / n:.0f}% ({100 * a:.0f}-{100 * b:.0f})"

def righe_tab(R, titolo):
    print(f"\n### {titolo}")
    print("| Gruppo | Righe del bersaglio | Letture | Nessuna riga | Classe giusta | Gruppo giusto | Sbagliata e sicura (≥50%) | Sbagliata e ≥90% |")
    print("|---|---|---|---|---|---|---|---|")
    for g in ("sub", "guerra", "civile"):
        for nome, f in (("almeno 1", lambda r: r["ntar"] >= 1), ("nessuna", lambda r: r["ntar"] == 0)):
            X = [r for r in R if r["gruppo"] == g and f(r)]
            Y = [r for r in X if r["nr"] > 0]
            print(f"| {g} | {nome} | {len(X)} | {sum(r['nr'] == 0 for r in X)} | {pc(sum(r['giusta'] for r in Y), len(Y))} | {pc(sum(r['giusto_g'] for r in Y), len(Y))} | "
                  f"{pc(sum(not r['giusta'] and r['conf'] >= .5 for r in Y), len(Y))} | {pc(sum(not r['giusta'] and r['conf'] >= .9 for r in Y), len(Y))} |")

def senza_baffles(R): return [r for r in R if not r["baffles"]]

def tutto():
    V = {v: senza_baffles(carica(v)) for v in ("oggi", "senza_s3", "raggi")}
    T = {v: carica(v) for v in V}
    print("# letture", {v: len(T[v]) for v in T}, "fuori dai baffles", {v: len(V[v]) for v in V})
    for v in V:
        S = [r for r in V[v] if r["tipo"] == "scena"]
        P = [r for r in V[v] if r["tipo"] == "spazzata"]
        righe_tab(S, f"{v}: scene vere, fascio sul rilevamento vero, 0-40 km")
        righe_tab(P, f"{v}: una classe alla volta, senza traffico né paesaggio")

    print("\n### Quanto ci si fida di quello che dice (oggi, scene): per righe osservate e righe che concordano")
    print("| Righe osservate | Letture | Classe giusta | Confidenza media | ≥90% e sbagliata |")
    print("|---|---|---|---|---|")
    S = [r for r in V["oggi"] if r["tipo"] == "scena" and r["nr"] > 0]
    for a, b in ((1, 1), (2, 2), (3, 3), (4, 5), (6, 9), (10, 99)):
        X = [r for r in S if a <= r["nr"] <= b]
        if X: print(f"| {a}-{b} | {len(X)} | {pc(sum(r['giusta'] for r in X), len(X))} | {sum(r['conf'] for r in X) / len(X):.2f} | {pc(sum(not r['giusta'] and r['conf'] >= .9 for r in X), len(X))} |")
    X = [r for r in S if r["conc"] == 0]
    print(f"\nVoce in testa senza nessuna riga concorde: {pc(len(X), len(S))}; di queste date a un sottomarino: {sum(r['gtop'] == 'sub' for r in X)}")

    print("\n### Da dove vengono le righe osservate (scene)")
    print("| Variante | Righe | del bersaglio | di altre sorgenti | proprie | senza origine |")
    print("|---|---|---|---|---|---|")
    for v in V:
        S = [r for r in V[v] if r["tipo"] == "scena"]
        n = sum(r["nr"] for r in S)
        print(f"| {v} | {n} | {sum(r['ntar'] for r in S)} | {sum(r['nalt'] for r in S)} | {sum(r['npro'] for r in S)} | {sum(r['nvuo'] for r in S)} |")

    print("\n### Le cause una alla volta (scene): il bersaglio solo")
    print("C: le righe del bersaglio ≥ 3 dB sul rumore, livello A. D: lo stesso coi raggi. E: lo spettro del bersaglio solo, col suo continuo, letto come la stazione; livello A oggi, raggi nella variante.")
    print("| Gruppo | Letture | Oggi, nel mare | C: righe, classe giusta | D: righe, classe giusta | E livello A: righe, classe giusta | E raggi: righe, classe giusta | Oggi coi raggi nel mare |")
    print("|---|---|---|---|---|---|---|---|")
    rag = {(r["scena"], r["seme"], r["traffico"], r["t"], r["id"]): r for r in V["raggi"] if r["tipo"] == "scena"}
    for g in ("sub", "guerra", "civile"):
        X = [r for r in V["oggi"] if r["tipo"] == "scena" and r["gruppo"] == g]
        n = len(X)
        if not n: continue
        k = lambda f: sum(f(r) for r in X)
        ER = [rag.get((r["scena"], r["seme"], r["traffico"], r["t"], r["id"])) for r in X]
        ER = [e for e in ER if e]
        print(f"| {g} | {n} | {pc(k(lambda r: r['giusta']), n)} | {sum(r['c_n'] for r in X) / n:.1f}, {pc(k(lambda r: r['c_top'] == r['classe'] and r['c_n'] > 0), n)} | "
              f"{sum(r['d_n'] for r in X) / n:.1f}, {pc(k(lambda r: r['d_top'] == r['classe'] and r['d_n'] > 0), n)} | "
              f"{sum(r['e_n'] for r in X) / n:.1f}, {pc(k(lambda r: r['e_top'] == r['classe'] and r['e_n'] > 0), n)} | "
              f"{sum(e['e_n'] for e in ER) / max(len(ER), 1):.1f}, {pc(sum(e['e_top'] == e['classe'] and e['e_n'] > 0 for e in ER), len(ER))} | "
              f"{pc(sum(e['giusta'] for e in ER), len(ER))} |")

    print("\n### La riga sul suo stesso continuo (dB nel bin da 0,1 Hz), spazzata: non dipende da distanza né perdita")
    print("| Gruppo | kn | Letture | Margine massimo: mediana | Righe a ≥3 dB: mediana | su righe emesse |")
    print("|---|---|---|---|---|---|")
    P = [r for r in T["oggi"] if r["tipo"] == "spazzata"]
    for g in ("sub", "guerra", "civile"):
        for kn in (5, 15, 25):
            X = sorted((r for r in P if r["gruppo"] == g and abs(r["v_kn"] - kn) < .1), key=lambda r: r["mg_max"])
            if not X: continue
            Y = sorted(r["mg_n"] for r in X); Z = sorted(r["mg_tot"] for r in X)
            print(f"| {g} | {kn} | {len(X)} | {X[len(X) // 2]['mg_max']:.1f} | {Y[len(Y) // 2]} | {Z[len(Z) // 2]} |")

    print("\n### Spazzata per distanza, oggi e coi raggi: classe giusta")
    print("| Gruppo | km | Oggi: righe medie, giusta | Raggi: righe medie, giusta |")
    print("|---|---|---|---|")
    PR = [r for r in V["raggi"] if r["tipo"] == "spazzata"]
    PO = [r for r in V["oggi"] if r["tipo"] == "spazzata"]
    for g in ("sub", "guerra", "civile"):
        for km in (1, 2, 5, 10, 20, 40):
            a = [r for r in PO if r["gruppo"] == g and abs(r["km"] - km) < .05]
            b = [r for r in PR if r["gruppo"] == g and abs(r["km"] - km) < .05]
            if a:
                print(f"| {g} | {km} | {sum(r['nr'] for r in a) / len(a):.1f}, {pc(sum(r['giusta'] for r in a), len(a))} | {sum(r['nr'] for r in b) / max(len(b), 1):.1f}, {pc(sum(r['giusta'] for r in b), len(b))} |")

    print("\n### Che cosa finirebbe nel registro col tasto 4 (classifica_dai_tracker), oggi")
    print("Scrive se il passo è PROBSUB o CERTSUB, o NONSUB con confidenza ≥ 50%. Giusta se la classe scritta è quella vera.")
    print("| Gruppo | Letture con righe | Scrive | Giusta | Errata | NONSUB su un sottomarino | PROB/CERTSUB su una nave |")
    print("|---|---|---|---|---|---|---|")
    for g in ("sub", "guerra", "civile"):
        X = [r for r in V["oggi"] if r["tipo"] == "scena" and r["gruppo"] == g and r["nr"] > 0]
        W = [r for r in X if r["passo"] in ("PROBSUB", "CERTSUB") or (r["passo"] == "NONSUB" and r["fconf"] >= .5)]
        print(f"| {g} | {len(X)} | {len(W)} | {sum(r['firma'] == r['classe'] for r in W)} | {sum(r['firma'] != r['classe'] for r in W)} | "
              f"{sum(r['passo'] == 'NONSUB' and r['fconf'] >= .5 for r in W) if g == 'sub' else '-'} | {sum(r['passo'] in ('PROBSUB', 'CERTSUB') for r in W) if g != 'sub' else '-'} |")

    print("\n### Il sonarista del giocatore sulle stesse unità (la partita): traccia classificata in quell'istante")
    print("| Gruppo | Tracce classificate | Con classe | Classe giusta | Errata per la partita |")
    print("|---|---|---|---|---|")
    for g in ("sub", "guerra", "civile"):
        X = [r for r in V["oggi"] if r["tipo"] == "scena" and r["gruppo"] == g and r["son_cl"] != "-"]
        C = [r for r in X if r["son_cl"] != "~"]
        print(f"| {g} | {len(X)} | {len(C)} | {pc(sum(r['son_cl'] == r['classe'] for r in C), len(C))} | {pc(sum(r['son_err'] == '1' for r in X), len(X))} |")

    print("\n### Le voci in testa più frequenti, scene, oggi e senza S3")
    for v in ("oggi", "senza_s3"):
        c = collections.Counter(r["top"] for r in V[v] if r["tipo"] == "scena" and r["nr"] > 0)
        print(v, c.most_common(10))

if __name__ == "__main__":
    tutto()
