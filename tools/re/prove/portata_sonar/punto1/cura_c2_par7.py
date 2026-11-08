#!/usr/bin/env python3
"""Sostituisce in C2 §7 il blocco del bordo dei raggi col testo di chiusura della portata (kit, 06/11/2026)."""
import re
f = 'docs/manuale_enhanced/2_riferimento/C2_propagazione.md'
t = open(f, encoding='utf-8').read()
nuovo = '''- **Il bordo dei raggi è a 300 km e ferma solo i bersagli più rumorosi.** Nella missione la griglia finisce lì: oltre,
  `CampoPerdita::tl` non dà nulla e vale sempre il livello A, con lo specchio di Lloyd e l'ombra che i raggi non hanno. Sotto
  i 100 km la griglia più lunga dà la stessa perdita di prima (0,00 dB su 191.040 punti).
  - **Portate statiche sul mercantile a 6 nodi** (giugno e ottobre, 805 m di fondale, ricevitore a 80 m): Skat con SE ≥ 0 a
    117 e 133 km, con SE ≥ +10 a 60 e 45 km; TB-16 con SE ≥ 0 a 192 e 202 km, con SE ≥ +10 a 103 e 111 km. Con la Skat, SE ≥ 0: cargo a 129 e 149 km, petroliera a
    163 e 179 km.
  - **In corsa, Victor sul mercantile** (giugno, 3 semi, 3000 s): a 40 km contatto solido in 110-135 s; a 99 km in 840-1650 s,
    con perdite di contatto; a 130 km solo in 1 seme su 3.
  - **Oltre i 100 km si sentono anche i sottomarini veloci.** Portate statiche del Victor a 15 nodi e 150 m: con la SQR-19 130-150 km
    su 805 m, fino al bordo su 3000 m; con la TB-16 80-110 km su 805 m, 110-150 km su 3000 m.
  - **Il bersaglio silenzioso si perde molto prima:** la Perry a 6 nodi, per la Skat, a 52-62 km.

  Misure: `tools/re/prove/portata_sonar/proposta_punto1.md`, `punto1/`.
- **La Perry è generosa sul Victor a 60 km.** Una Perry a 12-15 nodi, alle Fær Øer con 1817 m di fondale, ha il contatto solido su un
  Victor a 3 nodi e 300 m in 4 semi su 5 (3600 s). Il calcolo statico si ferma a 40 km (SQR-19, Perry a 6 nodi, 805 m), con
  SE circa −2 a 60 km: il resto lo fa la fluttuazione di 4 dB (`SIGMA_DB`). A 100 km, con 664 m di fondale, 0 semi su 5. C'era già
  con la griglia a 100 km; gli altri teatri non sono misurati.
- **Il traffico civile oltre i 30 km resta al livello A**, senza il campo di raggi (`TRAFFICO_CAMPO_M`, capitolo E10): una nave
  della scena e una del traffico non si sentono allo stesso modo. Sul mercantile a 6 nodi, a 40 km, in giugno, la Skat ha SE
  +11,5 coi raggi e −52 col livello A.
'''
a = t.index('- **Il bordo dei raggi a 100 km fa da limite di portata')
b = t.index('## 8. Riferimenti')
t = t[:a] + nuovo + '\n' + t[b:]
open(f, 'w', encoding='utf-8').write(t)
