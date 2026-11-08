#!/usr/bin/env python3
# Quali membri degli archivi hanno codice diverso fra due build: confronta le sezioni alloc non di debug (.text*, .rodata*, .data*)
import hashlib, os, subprocess, sys, tempfile
def impronte(lib):
    d = tempfile.mkdtemp(dir='mem'); subprocess.run(['ar', 'x', os.path.abspath(lib)], cwd=d, check=True)
    r = {}
    for n in os.listdir(d):
        p = os.path.join(d, n)
        sez = subprocess.run(['readelf', '-SW', p], capture_output=True, text=True).stdout
        h = hashlib.sha1()
        for riga in sez.splitlines():
            c = riga.replace('[', ' ').replace(']', ' ').split()
            if len(c) > 2 and c[1].startswith(('.text', '.rodata', '.data')) and 'debug' not in c[1] and c[2] == 'PROGBITS':
                o = int(c[4], 16); s = int(c[5], 16)
                with open(p, 'rb') as f:
                    f.seek(o); h.update(f.read(s))
        r[n] = (h.hexdigest(), os.path.getsize(p))
    subprocess.run(['rm', '-rf', d])
    return r
a, b = sys.argv[1], sys.argv[2]
for l in ['libnucleo.a', 'libmissioni.a']:
    x, y = impronte(f'{a}/{l}'), impronte(f'{b}/{l}')
    for n in sorted(set(x) | set(y)):
        if x.get(n, (0,))[0] != y.get(n, (0,))[0]:
            print(l, n, x.get(n, (0, 0))[1], y.get(n, (0, 0))[1])
