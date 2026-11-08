# le righe QUADRO della misura, per partita: salite, salite per ciclo, chiusure, copie, dato usato, che cosa ferma alle finestre
import re, sys, pathlib, collections
U = pathlib.Path('/home/randolph/Claude/melchior-688/engine/build_radio/' + __import__('os').environ.get('CARTELLA', 'misura'))
pref = sys.argv[1:] or ['']
for f in sorted(U.glob('*.txt')):
    if f.name in ('fatti.txt', 'lavori.txt', 'md5.txt', 'fine.txt', 'codici.txt', 'ctl_S15_1.txt') or not any(f.name.startswith(p) for p in pref):
        continue
    t = f.read_text(errors='replace').splitlines()
    sal = collections.Counter(); chiuse = collections.Counter(); cop = 0; us = 0; suby = collections.Counter()
    for l in t:
        if not l.startswith('QUADRO'):
            continue
        k = dict(x.split('=') for x in l.split()[2:] if '=' in x)
        if l.startswith('QUADRO salita'):
            sal[(k['id'], k['prossimo'])] += 1; suby[k['id']] += 1
        elif l.startswith('QUADRO chiusa'):
            for m in ('script', 'occupato', 'rimanda', 'quota_altri', 'vel_altri'):
                if k.get(m) == '1': chiuse[m] += 1
        elif l.startswith('QUADRO copia'):
            cop += 1
        elif l.startswith('QUADRO usato'):
            us += 1
    imp = next((re.search(r'dopo (\d+) s simulati \(impronta (\w+)', l).groups() for l in t if l.startswith('istantanea') and 'impronta' in l), ('?', '?'))
    es = next((l.strip() for l in t if l.strip().startswith('ESITO')), '-')
    print(f"{f.stem:9} salite {sum(sal.values()):3} battelli {len(suby):2} max/ciclo {max(sal.values(), default=0)} chiuse {dict(chiuse)} copie {cop:3} usato {us:2} | {imp[0]} s {imp[1][:8]} {es}")
