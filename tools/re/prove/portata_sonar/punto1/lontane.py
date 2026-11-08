#!/usr/bin/env python3
"""Per ogni PROVA_*: la coppia di unita' piu' lontana all'inizio (m), per dire quali scene restano sotto i 100 km."""
import glob, os, re, math
for f in sorted(glob.glob('/home/randolph/Claude/melchior-688/data/missioni/prove/PROVA_*.lua')):
    t = open(f).read()
    i = t.find('unita = {')
    pos = [(float(a), float(b)) for a, b in re.findall(r'posizione = \{ x = (-?[\d.]+), y = (-?[\d.]+) \}', t)]
    d = max((math.dist(a, b) for a in pos for b in pos), default=0)
    print(os.path.basename(f)[:-4], len(pos), round(d / 1000, 1))
