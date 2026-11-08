# Perche' Uc e' zero: dai cicli con messaggi nuovi via filo virtuale, quanti cadono su ogni condizione (solo lettura).
# Uso: perche_uc.py cartella partita...
import sys, pathlib, collections
B = pathlib.Path(sys.argv[1]); T = collections.Counter(); es = []
for n in sys.argv[2:]:
    virt = collections.defaultdict(set); c = collections.Counter()
    for l in (B / f'{n}.txt').read_text(errors='replace').splitlines():
        if not l.startswith('QUADRO ciclo'): continue
        a = l.split(); d = dict(x.split('=', 1) for x in a[2:] if '=' in x)
        msg = [x.split(':') for x in d.get('msg', '').split(',') if x]
        if not msg: continue
        c['cicli con msg'] += 1
        nuovi = [x for x in msg if (x[0], x[1]) not in virt[d['id']]]
        if int(d['a']) < 2: c['sotto A2'] += 1; continue
        if not nuovi: c['gia visti'] += 1; continue
        lim = int(d['lim'])
        if d['rotto'] == '1': c['filo rotto'] += 1; continue
        if lim == 0: c['fuori limiti'] += 1; continue
        for x in nuovi: virt[d['id']].add((x[0], x[1]))
        ost = [x for x in nuovi if x[2] == '1']
        if not ost: c['nuovi non ostili'] += 1; continue
        c['copia ostile'] += 1
        if d['esposto'] == '1': c['  gia albero'] += 1
        if d['libero'] == '1': c['  libero'] += 1
        us = [x for x in ost if x[3] == '1']
        if us: c['  usabile'] += 1
        if us and d['libero'] == '1': c['  USABILE E LIBERO'] += 1
        es.append((n, d['t'], d['id'], d['a'], d['quota'], d['vel'], d['libero'], d['esposto'], ost[:3]))
    print(n, dict(c)); T.update(c)
print('TOTALE', dict(T))
for e in es[:40]: print(e)
