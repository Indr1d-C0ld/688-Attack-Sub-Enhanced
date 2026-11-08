# I conti che completano attesi_a2.py: le righe flash per l'ELF e le partite con Uc alla lettera nelle scene. Solo lettura.
import sys, pathlib, collections
B = pathlib.Path(sys.argv[1])
def righe(n):
    for l in (B / f'{n}.txt').read_text(errors='replace').splitlines():
        if l.startswith('QUADRO '):
            a = l.split(); d = dict(x.split('=', 1) for x in a[2:] if '=' in x); d['_'] = a[1]; yield d
M18 = [f'{m}_{s}' for m in 'I02 I05 I06 I07 S03 S07 S08 S11 S15'.split() for s in (1, 2)]
for nome, lista in [('18 partite', M18)] + [(k, [f'{k}_{i}' for i in range(1, 11)]) for k in 'm4a m4b m4c s5 s5spia'.split()]:
    f = collections.Counter(); ul = 0; u = 0
    for n in lista:
        virt = collections.defaultdict(set); pu = pl = False
        for d in righe(n):
            if d['_'] == 'flash' and d['lato'] == '16':
                f['flash urss'] += 1
                if int(d['a']) >= 2:
                    f['in A2+'] += 1; q = float(d['quota']); v = float(d['vel'])
                    if q > 300 or v > 15: f['fuori da 300 m e 15 kn'] += 1
                    elif q <= 120 and v <= 12 and d['rotto'] == '0': f['il filo lo riceve'] += 1
                    else: f['E'] += 1
            elif d['_'] == 'ciclo':
                msg = [x.split(':') for x in d.get('msg', '').split(',') if x]
                nuovi = [x for x in msg if (x[0], x[1]) not in virt[d['id']]]
                if int(d['a']) >= 2 and int(d['lim']) > 0 and d['rotto'] == '0' and nuovi:
                    if d['libero'] == '1' and any(x[2] == '1' and float(x[4]) <= 30000 for x in nuovi): pl = True
                    if d['libero'] == '1' and any(x[3] == '1' for x in nuovi): pu = True
                    for x in nuovi: virt[d['id']].add((x[0], x[1]))
        ul += pl; u += pu
    print(nome, dict(f), '| partite con Uc alla lettera', ul, '| con Uc della sonda', u)
