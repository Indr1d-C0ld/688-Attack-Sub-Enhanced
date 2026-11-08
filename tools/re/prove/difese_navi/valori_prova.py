#!/usr/bin/env python3
"""I valori attesi della prova 6 (engine/test/prove_difese_navi.cpp), da calcolo_esiti.py. Uso: python3 valori_prova.py"""
import calcolo_esiti as c
N = {'krivak': (90, 2, 0, 0), 'slava': (90, 3, 0, 0), 'perry15': (15, 1, 1, 0), 'perry128': (128, 1, 1, 0), 'kidd': (16, 2, 2, 0),
     'iowa': (17, 1, 4, 1), 'spruance': (0, 1, 2, 0)}
S = {'uno': [0.0], 'due': [0.0, 4.0], 'quattro': [0.0, 4.0, 8.0, 12.0], 'giocatore': [0.0, 4.0, 20.0, 24.0]}
for n, d in N.items():
    for s, t in S.items():
        for v in (240.0, 450.0):
            for al in (0, 1, 2):
                co, p = c.salva(d, v, t, al)
                print(f'{n:9} {s:9} {v:5.0f} A{al} colpi {co:.4f} P>=1 {p:.4f}')
