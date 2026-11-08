# Il confronto fra la serie di P3 e la base di base2: partite identiche, trasmissioni, fix, contatti al giocatore, salite per ciclo. Solo lettura.
import pathlib, collections, re, sys, hashlib
P = pathlib.Path('/home/randolph/Claude/melchior-688/engine/build_radio/p3'); A = pathlib.Path('/home/randolph/Claude/melchior-688/engine/build_radio/base2')
nomi = [l.split()[0] for l in (A/'lavori_18.txt').read_text().splitlines()] + [l.split()[0] for l in (P/'lavori_scene.txt').read_text().splitlines()]
def senza_sonda(f):
    return [l for l in f.read_text(errors='replace').splitlines() if not l.startswith('QUADRO ')]
def q(f):
    v = []
    for l in f.read_text(errors='replace').splitlines():
        if l.startswith('QUADRO '):
            a = l.split(); d = dict(x.split('=', 1) for x in a[2:] if '=' in x); d['_'] = a[1]; v.append(d)
    return v
def impr(f):
    return hashlib.md5(f.read_bytes()).hexdigest()
print('partita | testo uguale | traccia uguale | rapporti base/p3 | trasmette per lato | fix per lato (rapporto) | fix_giocatore | datum')
tot = collections.Counter()
for n in nomi:
    a = A/f'{n}.txt'; p = P/f'{n}.txt'
    tu = senza_sonda(a) == senza_sonda(p)
    tt = impr(A/f'{n}.m688tr') == impr(P/f'{n}.m688tr')
    qa, qp = q(a), q(p)
    ra = sum(d['_'] == 'rapporto' for d in qa); rp = sum(d['_'] == 'rapporto' for d in qp)
    tr = collections.Counter(d['lato'] for d in qp if d['_'] == 'trasmette')
    fx = collections.Counter((d['lato'], d.get('rapporto')) for d in qp if d['_'] == 'fix')
    fg = [d for d in qp if d['_'] == 'fix_giocatore']; dt = [d for d in qp if d['_'] == 'datum']; es = [d for d in qp if d['_'] == 'esm']
    print(n, tu, tt, f'{ra}/{rp}', dict(tr), dict(fx), len(fg), len(dt), 'esm', len(es))
    if n[0] in 'IS' and n[1].isdigit():
        for k, v in tr.items(): tot['tx'+k] += v
        for k, v in fx.items():
            tot['fix'+k[0]+'r'+str(k[1])] += v
        tot['fg'] += len(fg); tot['datum'] += len(dt)
    # il limite di 60 s
    ul = {}
    for d in qp:
        if d['_'] == 'trasmette':
            t = float(d['t'])
            if d['id'] in ul and t - ul[d['id']] < 60: print('  sotto i 60 s', n, d)
            ul[d['id']] = t
print(dict(tot))
