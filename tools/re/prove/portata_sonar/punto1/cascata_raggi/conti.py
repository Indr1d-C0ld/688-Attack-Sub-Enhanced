#!/usr/bin/env python3
# Dai registri del binario v (CASCATA_LOG): tracker e contatti, vecchia cascata e nuova, stessa scena.
# Riga «vista»: unita' che da sola supera i +3 dB (la soglia del tracker). Tracker «falso»: agganciato al picco (mancati 0)
# senza nessuna unita' o sorgente che da sola superi +3 dB in quel fascio o nei due vicini.
import os, re, sys
O = sys.argv[1]
def leggi(n):
    p = f"{O}/{n}.log"
    if not os.path.exists(p): return None
    R, U, T = [], {}, []
    for l in open(p):
        x = l.split()
        if x[0] == "R": R.append({x[i]: float(x[i + 1]) for i in range(2, len(x) - 1, 2)} | {"t": float(x[1])})
        elif x[0] == "U": U.setdefault(float(x[1]), []).append({"id": int(x[2]), "classe": x[3], "km": float(x[5]), "fa": int(x[7]), "db": float(x[9]), "q": float(x[11])})
        elif x[0] == "T":
            b = x.index("|")
            T.append({"t": float(x[1]), "n": x[2], "ril": float(x[4]), "db": float(x[6]), "mancati": int(x[10]), "id": int(x[b + 1]), "classe": x[b + 2], "km": float(x[b + 4]), "solo": float(x[b + 6]), "contatto": int(x[b + 8])})
    return R, U, T
def conta(d):
    R, U, T = d
    ult = R[-1]
    tu = max(U) if U else None
    u = U.get(tu, []) if tu is not None else []
    solidi = [x for x in u if x["q"] >= 1000]
    visti = [x for x in u if x["db"] >= 3]
    # lungo tutta la partita
    falsi = [x for x in T if x["mancati"] == 0 and x["solo"] < 3]
    stantii = [x for x in T if x["mancati"] > 0 and x["solo"] < 3]
    nc = [x for x in T if not x["contatto"]]
    return {"nati": int(ult["nati"]), "attivi": int(ult["attivi"]), "fine_visti": len(visti), "fine_solidi": len(solidi),
            "solidi_visti": len([x for x in solidi if x["db"] >= 3]), "visti_noncont": len([x for x in visti if x["q"] < 0]),
            "fasci3": int(ult["fasci3"]), "fasci8": int(ult["fasci8"]), "fasci": int(ult["fasci"]), "righeT": len(T), "falsi": len(falsi), "stantii": len(stantii), "nc": len(nc), "unita_tr": sorted({x["id"] for x in T})}
print("scena | cascata | tracker nati, attivi a fine | a fine: unita' a +3, solidi, solidi visti, viste senza contatto | fasci >=3, >=8, su | righe tracker, false, stantie, senza contatto")
tot = {}
for n in ["s2b_si", "s2b_no", "s2a_si", "s2a_no", "s1a_si", "s1a_no", "s5a_si", "m3_si", "s5_si", "m4_si", "s7_si", "s3_si"]:
    uni = {}
    for v in ["vecchia", "dopo"]:
        d = leggi(f"{n}_{v}")
        if not d or not d[0]: print(n, v, "-"); continue
        c = conta(d); uni[v] = c
        print(f"{n:7} {v:8} | {c['nati']:3} {c['attivi']:2} | {c['fine_visti']:3} {c['fine_solidi']:3} {c['solidi_visti']:3} {c['visti_noncont']:3} | {c['fasci3']:2} {c['fasci8']:2} /{c['fasci']:2} | {c['righeT']:4} {c['falsi']:3} {c['stantii']:3} {c['nc']:3}")
    if len(uni) == 2:
        a, b = set(uni["vecchia"]["unita_tr"]), set(uni["dopo"]["unita_tr"])
        print(f"        unita' seguite da un tracker: solo prima {sorted(a - b)}; solo dopo {sorted(b - a)}; comuni {len(a & b)}")
