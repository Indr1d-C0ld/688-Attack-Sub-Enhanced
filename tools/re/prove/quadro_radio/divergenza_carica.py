# Dove si staccano le corse di S03 con la riga di stato: numeri casuali, poi per unita' posizione, quota, velocita' e carica. Solo lettura.
# Uso: divergenza_carica.py seme
import sys, pathlib, collections
U = pathlib.Path('/home/randolph/Claude/melchior-688/engine/build_radio')/(sys.argv[2] if len(sys.argv) > 2 else 'carica'); s = sys.argv[1]
def leggi(k):
    M = {}; S = {}
    for l in (U/f'{k}_{s}.txt').read_text(errors='replace').splitlines():
        if l.startswith('QUADRO mondo') or l.startswith('QUADRO stato'):
            a = l.split(); d = dict(x.split('=', 1) for x in a[2:])
            t = int(float(d['t']))
            if a[1] == 'mondo': M[t] = d
            else: S[(t, int(d['id']))] = d
    return M, S
Mb, Sb = leggi('b'); Mp, Sp = leggi('p')
print('fine', max(Mb), max(Mp))
for c in 'caso radio casuale reaz acu son'.split():
    t = next((t for t in sorted(Mb) if t in Mp and Mb[t][c] != Mp[t][c]), None)
    print('primo scarto del caso', c, t)
ids = sorted({i for (_, i) in Sb if i < 10000})
for i in ids:
    for c in 'x y quota vel vo rotta carica sn em ev ag nota'.split():
        t = next((t for t in sorted(Mb) if (t, i) in Sb and (t, i) in Sp and Sb[(t, i)][c] != Sp[(t, i)][c]), None)
        print('id', i, c, t, end=' | ')
    print()
print('t | b: carica sn em ev quota vel | p: carica sn em ev quota vel, Foxtrot 2')
for t in sorted(Mb):
    if t % 300 == 0 or 8300 <= t <= 8500 and t % 20 == 0 or t >= 9200 and t <= 9400 and t % 20 == 0:
        f = lambda S: ' '.join(S[(t, 2)][c] for c in 'carica sn em ev quota vel nota'.split()) if (t, 2) in S else '-'
        print(t, '|', f(Sb), '|', f(Sp))
