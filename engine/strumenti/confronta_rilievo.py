"""Confronta due rilievi di rilievo_missioni (righe separate da tabulazioni): partite diverse, esiti e totali.
uso: confronta_rilievo.py RIFERIMENTO NUOVO   (riferimento: engine/strumenti/riferimento/rilievo_missioni.tsv)"""
import csv, sys

def leggi(p): return {(r['scena'], r['seme']): r for r in csv.DictReader(open(p), delimiter='\t')}
a, b = leggi(sys.argv[1]), leggi(sys.argv[2])
colonne = [c for c in next(iter(b.values())) if c not in ('scena', 'seme')]
diverse = esiti = 0
for k in sorted(b):
    if k not in a: continue
    d = [f"{c} {a[k].get(c)} -> {b[k][c]}" for c in colonne if a[k].get(c) != b[k][c]]
    if d:
        diverse += 1; esiti += a[k]['esito'] != b[k]['esito']
        print(' '.join(k), '|', '; '.join(d))
somma = lambda t, c: sum(int(r[c]) for r in t.values())
att = lambda t, i: sum(int(r['attacchi'].split('/')[i]) for r in t.values())
print(f"{diverse} partite diverse su {len(b)}, {esiti} esiti diversi")
print(f"armi a segno {somma(a, 'armi')} -> {somma(b, 'armi')}; attacchi riusciti {att(a, 0)}/{att(a, 1)} -> {att(b, 0)}/{att(b, 1)}; "
      f"unita' distrutte {somma(a, 'distrutte')} -> {somma(b, 'distrutte')}")
