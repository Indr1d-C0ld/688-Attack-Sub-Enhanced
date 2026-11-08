#!/usr/bin/env python3
# riassunto_tentativo.py CARTELLA MODO: i numeri contro il criterio dell'esca, fissato il 05/11 prima delle corse.
# Uno scatto «perde il contatto» se il guadagno al minuto sul Victor nei minuti +15..+60 e' al massimo la meta' di quello nei -15..0, e se
# l'Alfa ne guadagna (qualcosa nei minuti 0..+60). Non misurabile: meno di 15 minuti dopo lo scatto prima della fine, o nessun contatto prima.
import glob, re, statistics as st, sys
FINE = 238
d, modo = sys.argv[1], sys.argv[2]
righe = []
for s in range(1, 13):
    testo = open(f'{d}/{modo}_{s}.txt').read()
    l = next(x for x in testo.splitlines() if x.startswith('seme'))
    righe.append((s, int(re.search(r'esito (-?\d+)', l)[1]), int(re.search(r'liberi (-?\d+)', l)[1]), l, testo))
lib = [r[2] for r in righe]
print(modo, 'liberi', *lib, 'media %.2f' % st.mean(lib), 'esito40 %d/12' % sum(r[1] == 40 for r in righe))
persi = misurabili = totali = 0
veloci = []
for s, _, _, _, testo in righe:
    for m in re.finditer(r"scatto seme \d+ t=\s*(\d+)' Victor (\d+) da (\d+) a [\d.]+ km, max [\d.]+(?: media ([\d.]+))? kn; Victor -15/0 (\d+), 0/15 (\d+), 15/30 (\d+), 30/60 (\d+); Alfa -15/0 (\d+), 0/15 (\d+), 15/30 (\d+), 30/60 (\d+)", testo):
        t0, vm, vp, v0, v15, v30, a0, a15, a30, a60 = int(m[1]), m[4], int(m[5]), int(m[6]), int(m[7]), int(m[8]), int(m[9]), int(m[10]), int(m[11]), int(m[12])
        totali += 1
        if vm: veloci.append(float(vm))
        dopo = min(FINE, t0 + 60) - (t0 + 15)
        if dopo < 15 or vp == 0: continue
        misurabili += 1
        perso = (v15 + v30) / dopo <= 0.5 * vp / 15 and a15 + a30 + a60 > 0
        persi += perso
print('scatti', totali, 'misurabili', misurabili, 'contatto perso', persi)
if veloci: print('velocita media degli scatti: da %.1f a %.1f, media %.1f' % (min(veloci), max(veloci), st.mean(veloci)))
print('pedina:', sum('pedina: ' in r[4] and bool(re.search(r"pedina: \d", r[4])) for r in righe), 'semi su 12')
