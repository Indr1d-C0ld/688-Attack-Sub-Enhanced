#!/usr/bin/env python3
"""Correzioni puntuali alla nota di chiusura: si ferma se un testo non c'e' esattamente una volta."""
p = '/home/randolph/.config/Claude/side-session-notes/local_54579f03-2ae3-4feb-a687-2bc2c6456a76/portata-punto1-chiusura-06-11.md'
t = open(p).read()
C = [
 ('| M4 | 21: 10 | 1,8 (base 0,9) | 1,2 | Victor sul mercantile solido 114-236 s, 10 su 10, uguale a S3 |',
  '| M4 | 21: 10 | 1,8 (base 1,1) | 1,2 (base 1,6), a segno 0,3 (0,9) | Victor sul mercantile solido 114-236 s, 10 su 10, uguale a S3 |'),
 ('Giro di 45 minuti sulle scene sì, serie S1 e S6 con B no.', 'Le 23 scene sì; le serie di S1 e S6 con B no.'),
 ('Testo: «Ai 300 km della griglia le unità veloci si sentono: contatto solido senza posizione nota, nessun lancio delle due flotte. Esito 21 in 10 su 10».',
  'Testo: «Ai 300 km della griglia le unità veloci si sentono: contatto solido senza posizione nota. Il Patto non lancia; il Los Angeles del comandante IA sì, da lontano (vedi sotto). Esito 21 in 10 su 10».'),
 ('Solido sul mercantile 114-236 s a 40 km invece di 83 s: **la riga «a 40 km ha il contatto solido a 83 s» ora è falsa**, resta vera per la petroliera a 85-119 s. Rileggere anche la riga a 85-95 km su `esiti/s3/corse/`.',
  'E6:1458, **«a 40 km ha il contatto solido a 83 s» ora è falsa**: il primo solido sul convoglio arriva a 85-119 s, sul mercantile a 114-236 s. Rileggere anche la riga a 85-95 km su `esiti/s3/corse/`. E6:1400 e 1405 sono misure datate: restano come storia, o con una nota.'),
 ('È solo dello strumento, ma va nel codice delle missioni: da assegnare.', 'Sta in `missioni/src/comandante_ia.cpp`, usato dalle serie: da assegnare.'),
]
for a, b in C:
    assert t.count(a) == 1, a
    t = t.replace(a, b)
open(p, 'w').write(t)
print('ok', len(C))
