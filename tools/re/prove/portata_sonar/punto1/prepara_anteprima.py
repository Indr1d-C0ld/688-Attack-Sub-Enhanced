#!/usr/bin/env python3
"""L'anteprima delle sorgenti, prima del codice: una cartella accanto, con data/motore e data/suoni a simboli e solo
acustica.toml copiato, con punti e tonale delle quattro firme civili abbassati di 17 dB. Abbassare i punti da' lo stesso
spettro che dara' il codice; non tocca data/."""
import os, re, sys
R = '/home/randolph/Claude/melchior-688'
U = os.path.join(R, 'tools/re/prove/portata_sonar/punto1/anteprima')
M = os.path.join(U, 'motore')
os.makedirs(M, exist_ok=True)
if not os.path.lexists(os.path.join(U, 'suoni')): os.symlink(os.path.join(R, 'data/suoni'), os.path.join(U, 'suoni'))
for n in os.listdir(os.path.join(R, 'data/motore')):
    if n != 'acustica.toml' and not os.path.lexists(os.path.join(M, n)):
        os.symlink(os.path.join(R, 'data/motore', n), os.path.join(M, n))
t = open(os.path.join(R, 'data/motore/acustica.toml')).read()
n = 0
for c in ('mercantile', 'cargo', 'petroliera', 'peschereccio'):
    m = re.search(r'\[firme\.%s\]\npunti = (\[.*\])\n' % c, t)
    pts = eval(m.group(1))
    nuovi = [[v, l - 17.0] for v, l in pts]
    a, b = m.span(1)
    t = t[:a] + repr(nuovi) + t[b:]
    i = t.index('[firme.%s]' % c)
    m2 = re.compile(r'tonale_db = ([\d.]+)').search(t, i)
    t = t[:m2.start(1)] + repr(float(m2.group(1)) - 17.0) + t[m2.end(1):]
    n += 1
open(os.path.join(M, 'acustica.toml'), 'w').write(t)
print('firme cambiate', n)
