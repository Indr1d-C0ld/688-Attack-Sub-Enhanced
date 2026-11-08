"""Misure a monte per le sorgenti nuove del punto M: durata, ritmo (campioni_analisi.ritmo), picco dello spettro"""
import sys, os
sys.path.insert(0, '/home/randolph/Claude/melchior-688/tools/pipeline')
import numpy as np
from scipy import signal
import campioni_analisi as A
for p in sys.argv[1:]:
    x, fs = A.leggi(p)
    f, P = signal.welch(x, fs, nperseg=8192)
    m = (f > 20) & (f < 0.45 * fs)
    picco = f[m][np.argmax(P[m])]
    cum = np.cumsum(P[m]) / np.sum(P[m])
    print(f'{os.path.basename(p)}: {len(x)/fs:.1f} s a {fs} Hz; ritmo {A.ritmo(x, fs, 1.0)}; picco {picco:.0f} Hz; 10-90% energia {f[m][np.searchsorted(cum, 0.1)]:.0f}-{f[m][np.searchsorted(cum, 0.9)]:.0f} Hz')
