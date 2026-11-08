# S5: alle finestre, che cosa ferma il Victor quando il contatto solido e' oltre 15 km; per ciclo la combinazione piu' frequente
import pathlib, collections, sys
U = pathlib.Path('/home/randolph/Claude/melchior-688/engine/build_radio/misura')
for n in sys.argv[1:]:
    per = collections.defaultdict(list)
    for l in (U / f'{n}.txt').read_text(errors='replace').splitlines():
        if l.startswith('QUADRO finestra'):
            k = dict(x.split('=') for x in l.split()[2:])
            per[k['prossimo']].append(k)
    print(n, 'cicli guardati', len(per))
    for p, v in sorted(per.items(), key=lambda x: float(x[0])):
        cl = collections.Counter()
        for k in v:
            for c in ('script','affonda','difesa','reazione','tma','aggancio','missile','pedina','vetrino','evasione','rimanda','tentato','banchisa'):
                if k[c] == '1': cl[c] += 1
        vic = [float(k['vicino']) for k in v if float(k['vicino']) < 1e17]
        lontani = [k for k in v if float(k['vicino']) > 15000 or float(k['vicino']) > 1e17]
        c2 = collections.Counter()
        for k in lontani:
            c2[','.join(c for c in ('script','affonda','difesa','reazione','tma','aggancio','missile','pedina','vetrino','evasione','rimanda','tentato','banchisa') if k[c]=='1') or 'NESSUNA'] += 1
        print(f"  ciclo {p:>6} righe {len(v):4} t {v[0]['t']}-{v[-1]['t']} vicino {min(vic) if vic else -1:.0f}-{max(vic) if vic else -1:.0f} | {dict(cl)} | oltre 15 km: {dict(c2)}")
