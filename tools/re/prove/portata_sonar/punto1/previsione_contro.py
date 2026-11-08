#!/usr/bin/env python3
"""Attesi prima del codice: le contro-scoperte se un contatto che torna entro 30 s dal suo perso non conta come nuovo."""
import re, os
U = '/home/randolph/Claude/melchior-688/tools/re/prove/portata_sonar/punto1/esiti/contro'
for c in sorted(os.listdir(U)):
    if not os.path.isdir(f'{U}/{c}'): continue
    v = []
    for s in range(1, 11):
        perso = {}; X = set(); n = 0; righe = []
        for l in open(f'{U}/{c}/{s}.txt'):
            m = re.match(r'X t ([\d.]+) oss (-?\d+) su (-?\d+)', l)
            if m: X.add((float(m[1]), int(m[2]), int(m[3])))
            m = re.match(r'C t ([\d.]+) (\S+) oss (-?\d+) su (-?\d+)', l)
            if m: righe.append((float(m[1]), m[2], int(m[3]), int(m[4])))
        for t, tipo, o, b in righe:
            if tipo == 'contatto_perso': perso[(o, b)] = t
            elif (t, o, b) in X and not ((o, b) in perso and t - perso[(o, b)] < 30.0): n += 1
        v.append((len(X), n))
    print(c, 'prima, dopo per seme:', v)
