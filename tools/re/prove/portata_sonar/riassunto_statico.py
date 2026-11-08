# la portata calcolata dai TSV di sonda_portata statico: ultima distanza con SE >= 0 e con SE >= +10, per ricevitore e bersaglio
import math
import sys


def carica(f):
    righe = [l.rstrip('\n').split('\t') for l in open(f, encoding='utf-8') if not l.startswith('#') and l.strip()]
    h = righe[0]
    return [dict(zip(h, r)) for r in righe[1:] if len(r) == len(h) and r[0] != 'TL' and r[0] != 'ricevitore']


def portata(s, soglia, delta=0.0):
    # l'ultima distanza sopra la soglia, interpolando in log r col punto dopo
    out = 0.0
    for a, b in zip(s, s[1:] + [None]):
        sa = a[1] + delta
        if sa < soglia:
            continue
        out = a[0]
        if b is not None and b[1] + delta < soglia:
            sb = b[1] + delta
            out = a[0] * (b[0] / a[0]) ** ((sa - soglia) / (sa - sb))
    return out


def fom_sferica(fom_db, alfa_db_km):
    # la distanza in km dove 20 log r(m) + alfa r = FOM
    lo, hi = 0.1, 5000.0
    for _ in range(80):
        m = math.sqrt(lo * hi)
        if 20 * math.log10(m * 1000) + alfa_db_km * m > fom_db:
            hi = m
        else:
            lo = m
    return lo


for f in sys.argv[1:]:
    d = carica(f)
    print('==', f)
    print('%-14s %-11s %3s | %7s %7s | %7s %7s | %7s %7s | %7s %7s' % ('ricevitore', 'bersaglio', 'kn', 'B+A>=0', 'B+A>=10', 'raggi>=0', '>=10', '241m>=0', '>=10', '-20dB>=0', '>=10'))
    chiavi = sorted({(x['ricevitore'], x['bersaglio'], x['v_kn']) for x in d}, key=lambda k: (k[0], k[1], float(k[2])))
    for k in chiavi:
        serie = {}
        for liv in ('B', 'Bx', 'Bx241'):
            serie[liv] = sorted((float(x['km']), float(x['SE'])) for x in d if (x['ricevitore'], x['bersaglio'], x['v_kn']) == k and x['livello'] == liv)
        print('%-14s %-11s %3s | %7.0f %7.0f | %7.0f %7.0f | %7.0f %7.0f | %7.0f %7.0f' % (
            k[0], k[1], k[2], portata(serie['B'], 0), portata(serie['B'], 10), portata(serie['Bx'], 0), portata(serie['Bx'], 10),
            portata(serie['Bx241'], 0), portata(serie['Bx241'], 10), portata(serie['Bx'], 0, -20), portata(serie['Bx'], 10, -20)))
    print('\nFOM a 99 km (SL - (NL - DI) - DT), TL dei raggi e TL dei due estremi: sferica 20 log r, cilindrica con r0 = fondale')
    print('%-14s %-11s %3s %6s %5s %6s %5s %5s %5s %6s %6s %6s %6s | %8s' % ('ricevitore', 'bersaglio', 'kn', 'banda', 'lofar', 'SL', 'NL', 'DI', 'DT', 'FOM', 'TLragg', 'TLsfer', 'TLcil', 'r_sfer_km'))
    for k in chiavi:
        x = [y for y in d if (y['ricevitore'], y['bersaglio'], y['v_kn']) == k and y['livello'] == 'Bx' and float(y['km']) == 99.0][0]
        sl, nl, di, dt, tl = (float(x[c]) for c in ('SL', 'NL', 'DI', 'DT', 'TL'))
        fom = sl - (nl - di) - dt
        if int(x['lofar']):
            print('%-14s %-11s %3s  (riga LOFAR %s Hz, b nel DT: FOM a mano non confrontabile)' % (k[0], k[1], k[2], x['banda_hz']))
            continue
        r0 = 800.0
        alfa = 0.014 if float(x['banda_hz']) <= 300 else 0.064   # dB/km a 300 Hz e 1 kHz (C2 §2.5, 4 C)
        tl_sf = 20 * math.log10(99000.0) + alfa * 99
        tl_ci = 20 * math.log10(r0) + 10 * math.log10(99000.0 / r0) + alfa * 99
        print('%-14s %-11s %3s %6.0f %5s %6.1f %5.1f %5.1f %5.1f %6.1f %6.1f %6.1f %6.1f | %8.0f' % (k[0], k[1], k[2], float(x['banda_hz']), x['lofar'], sl, nl, di, dt, fom, tl, tl_sf, tl_ci, fom_sferica(fom, alfa)))
    print()
