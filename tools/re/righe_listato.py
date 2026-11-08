"""Stampa le righe di un listato SEG:OFF comprese fra due offset esadecimali. uso: righe_listato.py LISTATO A B"""
import sys
a, b = int(sys.argv[2], 16), int(sys.argv[3], 16)
for l in open(sys.argv[1]):
    try: v = int(l.split()[0].split(':')[1], 16)
    except Exception: continue
    if a <= v < b: print(l, end='')
