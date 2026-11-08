#!/usr/bin/env python3
"""Dalle tracce di una serie: per un osservatore e un insieme di bersagli, primo contatto, solido, posizione nota, per seme.
Uso: contatti_serie.py SONDA_TRACCE CARTELLA OSSERVATORE BERSAGLI (es. 2,3,4,5,6,7)"""
import glob, os, re, subprocess, sys
sonda, cartella, oss, bers = sys.argv[1:5]
bers = [int(x) for x in bers.split(',')]
env = dict(os.environ, LD_LIBRARY_PATH='/home/randolph/Claude/melchior-688/tools/deps/sysroot/usr/lib/x86_64-linux-gnu')
tot = {b: [] for b in bers}; trovato = 0; n = 0
for f in sorted(glob.glob(cartella + '/partita_*.m688tr'), key=lambda x: int(re.findall(r'(\d+)\.m688tr', x)[0])):
    r = {}
    for l in subprocess.run([sonda, f, oss], capture_output=True, text=True, env=env).stdout.splitlines()[1:]:
        p = l.split()
        if len(p) >= 8: r[int(p[0])] = (float(p[1]), float(p[3]), float(p[5]), int(p[7]))
    n += 1
    trovato += any(r.get(b, (-1, -1))[1] >= 0 for b in bers)
    for b in bers: tot[b].append(r.get(b, (-1, -1, -1, 0)))
print('oss', oss, 'semi', n, 'con almeno un bersaglio solido:', trovato)
for b in bers:
    print(' bers %d primo %s' % (b, ' '.join('%.0f' % x[0] for x in tot[b])))
    print('        solido %s' % ' '.join('%.0f' % x[1] for x in tot[b]))
    print('        pos    %s' % ' '.join('%.0f' % x[2] for x in tot[b]))
