# Lanci sul solo dato radio (§9.4, §9.8): per ogni QUADRO usato, i lanci di quel battello dalla riga fino alla fine della caccia, col contatto che aveva. Solo lettura.
# Uso: lanci_su_dato.py cartella partita...
import sys, pathlib, runpy
m = runpy.run_path('/home/randolph/Claude/melchior-688/tools/re/prove/quadro_radio/occasioni_tracce.py', run_name='lib')
B = pathlib.Path(sys.argv[1])
for n in sys.argv[2:]:
    us = []; la = {}
    for l in (B / f'{n}.txt').read_text(errors='replace').splitlines():
        if l.startswith('QUADRO usato') or l.startswith('QUADRO lasciato'):
            a = l.split(); d = dict(x.split('=', 1) for x in a[2:] if '=' in x); (us.append((float(d['t']), int(d['id']), int(d['su']))) if a[1] == 'usato' else la.setdefault((int(d['id']), int(d['su'])), float(d['t'])))
    if not us: continue
    print(n, 'usati', us, 'lasciati', la, 'lanci nel txt:')
    for l in (B / f'{n}.txt').read_text(errors='replace').splitlines():
        if not l.startswith('QUADRO') and ('lancia' in l.lower() or 'tubo' in l.lower()) and any(f'[{i}' in l or f' {i} ' in l for _, i, _ in us): print('  ', l[:130])
