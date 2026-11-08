# le corse di sonda_portata in una tabella per data, modo e distanza: primo contatto, contatto solido, posizione nota, sensore,
# e il SE nudo mediano del miglior sensore nei primi 600 s; una riga per gruppo convoglio (id 2, 3, 4) e una per la Perry (id 5)
import glob
import os
import re
import statistics
import sys

base = sys.argv[1] if len(sys.argv) > 1 else os.path.join(os.path.dirname(__file__), 'esiti/corse')
righe = {}
for f in sorted(glob.glob(os.path.join(base, '*.tsv'))):
    m = re.match(r'(\w+?)_(ia_quota|ia|g)_k(\d+)_s(\d+)\.tsv', os.path.basename(f))
    if not m:
        continue
    data, modo, km, seme = m.group(1), m.group(2), int(m.group(3)), int(m.group(4))
    testo = open(f, encoding='utf-8').read().splitlines()
    fine = next((float(l.split('t_fine=')[1]) for l in testo if l.startswith('# riassunto')), None)
    if fine is None:
        continue
    r = {}
    for l in testo:
        if l.startswith('# ') and re.match(r'# \d ', l):
            p = l[2:].split()
            # id classe primo d sens  q799 solido d sens  pos d err  perdite se_max (i sensori possono mancare)
            ident, classe = int(p[0]), p[1]
            numeri = [x for x in p[2:]]
            primo = float(numeri[0]); d_primo = float(numeri[1])
            i = 2
            sens1 = ''
            if primo >= 0:
                sens1 = numeri[i]; i += 1
            q799 = float(numeri[i]); solido = float(numeri[i + 1]); d_sol = float(numeri[i + 2]); i += 3
            sens2 = ''
            if solido >= 0:
                sens2 = numeri[i]; i += 1
            pos = float(numeri[i]); d_pos = float(numeri[i + 1]); err = float(numeri[i + 2]); perdite = int(numeri[i + 3]); se_max = float(numeri[i + 4])
            r[ident] = dict(classe=classe, primo=primo, d_primo=d_primo, sens1=sens1, q799=q799, solido=solido, d_sol=d_sol, sens2=sens2, pos=pos, d_pos=d_pos, err=err, perdite=perdite, se_max=se_max)
    # il SE nudo del miglior sensore nei primi 600 s, e l'ultima quota della schiera
    h = testo[0].split('\t')
    nudo = {}
    q_sch = None
    v_max = {}
    for l in testo[1:]:
        c = l.split('\t')
        if len(c) != len(h):
            continue
        d = dict(zip(h, c))
        if float(d['t']) <= 600:
            nudo.setdefault(int(d['bers']), []).append((float(d['SE_nudo']), d['migliore'], d['banda'], float(d['TL'])))
        q_sch = d['q_schiera']
        v_max[int(d['bers'])] = max(v_max.get(int(d['bers']), 0), float(d['v_bers']))
    righe[(data, modo, km, seme)] = (fine, r, nudo, q_sch, v_max)

print('data modo km seme fine | bers: primo_s(d_km,sensore) solido_s(d_km,sensore) pos_s(err_km) perdite SE_nudo_600s[sens,banda,TL]')
for k in sorted(righe, key=lambda x: (x[0], x[1], x[2], x[3])):
    fine, r, nudo, q_sch, v_max = righe[k]
    print('%s %s %3d s%d fine=%.0f schiera=%s' % (k[0], k[1], k[2], k[3], fine, q_sch))
    for b in sorted(r):
        e = r[b]
        n = nudo.get(b, [])
        se = statistics.median(x[0] for x in n) if n else float('nan')
        ult = n[len(n) // 2] if n else (0, '-', '-', 0)
        print('   %d %-10s primo %6.0f (%5.1f %-14s) solido %6.0f (%5.1f %-14s) pos %6.0f (%.1f) perdite %2d SE600 %+6.1f [%s %s TL%.0f] vmax %.0f' % (
            b, e['classe'], e['primo'], e['d_primo'], e['sens1'], e['solido'], e['d_sol'], e['sens2'], e['pos'], e['err'], e['perdite'], se, ult[1], ult[2], ult[3], v_max.get(b, 0)))
