"""Verifica della coda del capodoglio sul segnale (punto M): i click dall'inviluppo passa-alto, gli intervalli, i gruppi.
Una coda: 3-20 click con intervalli regolari e una pausa netta prima e dopo; i rapid clicks: un treno continuo."""
import sys
import numpy as np
from scipy import signal
import av

def leggi(p):
    with av.open(p) as f:
        s = next(x for x in f.streams if x.type == 'audio')
        fs = s.rate
        x = np.concatenate([fr.to_ndarray().astype(np.float64).mean(axis=0) for fr in f.decode(s)])
    return x, fs

x, fs = leggi(sys.argv[1])
print(f'{len(x)/fs:.1f} s a {fs} Hz')
y = signal.sosfiltfilt(signal.butter(4, [2000, min(20000, 0.45*fs)], btype='band', fs=fs, output='sos'), x)
env = np.abs(signal.hilbert(y))
env = signal.sosfiltfilt(signal.butter(2, 300, fs=fs, output='sos'), env)
fondo = np.median(env)
picchi, _ = signal.find_peaks(env, height=6 * fondo, distance=int(0.008 * fs))
t = picchi / fs
ici = np.diff(t)
print('click', len(t), 'fondo', f'{fondo:.2e}')
print('ICI mediano %.3f s, quantili 10/50/90: %s' % (np.median(ici), np.round(np.quantile(ici, [0.1, 0.5, 0.9]), 3)))
# ritmo nel tempo: click per secondo
cps = np.histogram(t, bins=np.arange(0, len(x)/fs + 1, 1.0))[0]
print('click al secondo:', ' '.join(str(c) for c in cps))
# gruppi: separati da pause > 3x l'ICI mediano dei vicini e > 0.5 s
gruppi, g = [], [t[0]] if len(t) else []
for a, b in zip(t[:-1], t[1:]):
    if b - a > 0.5: gruppi.append(g); g = [b]
    else: g.append(b)
if g: gruppi.append(g)
print('gruppi separati da pause > 0,5 s:', len(gruppi))
for g in gruppi:
    d = np.diff(g)
    cv = (np.std(d) / np.mean(d)) if len(d) > 1 else 0
    if 3 <= len(g) <= 20:
        print(f'  candidato coda a {g[0]:6.2f} s: {len(g)} click in {g[-1]-g[0]:.2f} s, ICI {np.round(d,3)}, CV {cv:.2f}')
print('gruppi lunghi (>20 click):', sum(len(g) > 20 for g in gruppi), '; corti (<3):', sum(len(g) < 3 for g in gruppi))
print('candidati regolari (CV < 0,3) con le pause prima e dopo:')
for i, g in enumerate(gruppi):
    d = np.diff(g)
    if 3 <= len(g) <= 20 and len(d) > 1 and np.std(d) / np.mean(d) < 0.3:
        prima = g[0] - gruppi[i - 1][-1] if i > 0 else float('nan')
        dopo = gruppi[i + 1][0] - g[-1] if i + 1 < len(gruppi) else float('nan')
        livello = 20 * np.log10(np.median(env[(np.array(g) * fs).astype(int)]) / fondo)
        print(f'  {g[0]:6.2f} s: {len(g)} click, ICI medio {np.mean(d):.3f} s, pausa prima {prima:.2f} s, dopo {dopo:.2f} s, {livello:.1f} dB sul fondo')
