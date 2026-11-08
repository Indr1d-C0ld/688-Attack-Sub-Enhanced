#!/usr/bin/env python3
# S22: esito e battelli liberi per seme, a 4 e a 20 nodi; i semi 1-12 separati dai 13-36.
import math, random, re, sys
from pathlib import Path

CARTELLA = Path(sys.argv[1] if len(sys.argv) > 1 else ".")
RIGA = re.compile(r"esito (\d+) liberi (\d+) .*tenuti: 0<-\{([^}]*)\}")


def leggi(prefisso, seme):
    with open(CARTELLA / f"{prefisso}_{seme}.txt") as f:
        m = RIGA.search(f.readline())
    alfa = {int(a): int(b) for a, b in re.findall(r"(\d+)@(\d+)", m.group(3))}
    return int(m.group(1)), int(m.group(2)), alfa


def wilson(k, n, z=1.959964):
    p = k / n
    d = 1 + z * z / n
    c = (p + z * z / (2 * n)) / d
    m = z * math.sqrt(p * (1 - p) / n + z * z / (4 * n * n)) / d
    return max(0, c - m), min(1, c + m)


def newcombe_appaiato(a, b, c, d):
    # a: entrambi, b: solo corsa, c: solo nascosto, d: nessuno (Newcombe 1998, metodo 10)
    n = a + b + c + d
    p1, p2 = (a + b) / n, (a + c) / n
    l1, u1 = wilson(a + b, n)
    l2, u2 = wilson(a + c, n)
    x = (a + b) * (c + d) * (a + c) * (b + d)
    y = a * d - b * c
    phi = 0 if x == 0 else (y - n / 2 if y > n / 2 else (0 if y >= 0 else y)) / math.sqrt(x)
    th = p1 - p2
    dl = math.sqrt((p1 - l1) ** 2 - 2 * phi * (p1 - l1) * (u2 - p2) + (u2 - p2) ** 2)
    du = math.sqrt((u1 - p1) ** 2 - 2 * phi * (u1 - p1) * (p2 - l2) + (p2 - l2) ** 2)
    return th - dl, th + du


def segno_esatto(s, g):
    n = s + g
    if n == 0:
        return 1.0
    k = min(s, g)
    return min(1.0, 2 * sum(math.comb(n, i) for i in range(k + 1)) / 2 ** n)


def boot_media(dif, giri=20000):
    r = random.Random(688)
    n = len(dif)
    v = sorted(sum(r.choice(dif) for _ in range(n)) / n for _ in range(giri))
    return v[int(0.025 * giri)], v[int(0.975 * giri)]


def rapporto(nome, semi):
    print(f"== {nome}, semi {semi[0]}-{semi[-1]} ({len(semi)})")
    N, C = {}, {}
    for s in semi:
        N[s], C[s] = leggi("nascosto", s), leggi("corsa", s)
    n = len(semi)
    for tag, D in (("4 nodi ", N), ("20 nodi", C)):
        lib = [D[s][1] for s in semi]
        k = sum(D[s][0] == 40 for s in semi)
        l, u = wilson(k, n)
        print(f"{tag}: liberi {' '.join(map(str, lib))}")
        print(f"  media {sum(lib) / n:.3f} esito40 {k}/{n} = {100 * k / n:.0f}% Wilson {100 * l:.0f}-{100 * u:.0f}%"
              f" semi {[s for s in semi if D[s][0] == 40]}"
              f" distrib {[lib.count(i) for i in range(5)]}")
        print(f"  coerenza esito/liberi: {all((D[s][0] == 40) == (D[s][1] >= 3) for s in semi)}"
              f" Alfa tenuto {sum(bool(D[s][2]) for s in semi)}/{n}"
              f" dalla Kidd {sum(9 in D[s][2] for s in semi)}"
              f" primo min {min(min(D[s][2].values()) for s in semi if D[s][2])}-{max(min(D[s][2].values()) for s in semi if D[s][2])}")
    ent = sum(N[s][0] == 40 and C[s][0] == 40 for s in semi)
    sc = sum(N[s][0] != 40 and C[s][0] == 40 for s in semi)
    sn = sum(N[s][0] == 40 and C[s][0] != 40 for s in semi)
    nes = n - ent - sc - sn
    l, u = newcombe_appaiato(ent, sc, sn, nes)
    print(f"appaiate: entrambi {ent}, solo corsa {sc}, solo nascosto {sn}, nessuno {nes}"
          f" -> diff {100 * (sc - sn) / n:+.0f} punti, IC {100 * l:+.0f}..{100 * u:+.0f}, segno esatto p={segno_esatto(sc, sn):.3f}")
    print(f"  solo corsa {[s for s in semi if N[s][0] != 40 and C[s][0] == 40]}"
          f" solo nascosto {[s for s in semi if N[s][0] == 40 and C[s][0] != 40]}")
    dif = [C[s][1] - N[s][1] for s in semi]
    su, giu = sum(d > 0 for d in dif), sum(d < 0 for d in dif)
    l, u = boot_media(dif)
    print(f"  differenze liberi {' '.join(map(str, dif))}")
    print(f"  media diff {sum(dif) / n:+.3f} boot95 {l:+.2f}..{u:+.2f}; su {su} giu {giu} pari {n - su - giu}, segno p={segno_esatto(su, giu):.3f}")


rapporto("ipotesi", list(range(1, 13)))
rapporto("prova", list(range(13, 37)))
rapporto("tutti", list(range(1, 37)))
