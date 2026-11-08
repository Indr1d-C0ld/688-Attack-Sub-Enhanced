#!/usr/bin/env python3
"""Riassume le righe di sonda_contro: contro-scoperte per seme, coppie, unita', sensori, assenze fra perso e nuovo."""
import re, collections, os, statistics as st
U = '/home/randolph/Claude/melchior-688/tools/re/prove/portata_sonar/punto1/esiti/contro'
for c in sorted(os.listdir(U)):
    if not os.path.isdir(f'{U}/{c}'): continue
    print('==', c)
    for s in range(1, 11):
        X = []; C = collections.defaultdict(list); classi = {}; D = 0
        for l in open(f'{U}/{c}/{s}.txt'):
            m = re.match(r'X t ([\d.]+) oss (-?\d+) su (-?\d+) sens (\S*) em (-?\d+)', l)
            if m: X.append((float(m[1]), int(m[2]), int(m[3]), m[4], int(m[5])))
            m = re.match(r'C t ([\d.]+) (\S+) oss (-?\d+) su (-?\d+)', l)
            if m: C[(int(m[3]), int(m[4]))].append((float(m[1]), m[2]))
            m = re.match(r'U (\d+) .* classe (\S+)', l)
            if m: classi[int(m[1])] = m[2]
        coppie = collections.Counter((x[1], x[2]) for x in X)
        gaps = []
        for k in coppie:
            ev = C[k]
            for a, b in zip(ev, ev[1:]):
                if a[1] == 'contatto_perso' and b[1] == 'contatto_nuovo': gaps.append(b[0] - a[0])
        un = collections.Counter(x[2] for x in X)
        sens = collections.Counter(x[3] or '-' for x in X)
        print(f' s{s}: contro {len(X)}, coppie {len(coppie)}, unita {[(classi.get(k, k), v) for k, v in un.most_common(3)]}, sensori {sens.most_common(2)}, assenza mediana {st.median(gaps) if gaps else 0:.2f} s, sotto 1 s {sum(g < 1 for g in gaps)}/{len(gaps)}, top {coppie.most_common(2)}')
