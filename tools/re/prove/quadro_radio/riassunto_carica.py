# Per S03, seme per seme e per regola: esito, fine, quando il Foxtrot passa a em=1, carica alla fine, rapporti. Solo lettura.
# Uso: riassunto_carica.py cartella semi...
import sys, pathlib, re
U = pathlib.Path('/home/randolph/Claude/melchior-688/engine/build_radio')/sys.argv[1]
for s in sys.argv[2:]:
    for k in 'bp':
        f = U/f'{k}_{s}.txt'; esito = fine = None; em = {}; ca = {}; rap = 0; sn = {}; tx = 0
        for l in f.open(errors='replace'):
            if l.startswith('QUADRO stato'):
                a = l.split()
                if a[3] not in ('id=2', 'id=3'): continue
                d = dict(x.split('=', 1) for x in a[2:]); i = d['id']; t = float(d['t'])
                ca[i] = float(d['carica'])
                if d['em'] == '1' and i not in em: em[i] = t
            elif l.startswith('QUADRO rapporto'): rap += 1
            elif l.startswith('QUADRO trasmette'): tx += 1
            elif l.startswith('  ESITO'): esito = l.split()[1]
            elif l.startswith('istantanea'): fine = re.search(r'dopo (\d+) s', l).group(1)
        print(s, k, 'esito', esito, 'fine', fine, 'em', em, 'carica', {i: round(v, 4) for i, v in ca.items()}, 'rapporti', rap, 'tx', tx)
