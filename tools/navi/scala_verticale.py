"""Stima del fattore di scala verticale di un disegno di profilo usando piu' foto indipendenti.

Per ogni k candidato le quote z dei punti di riferimento (prese dal disegno) vengono moltiplicate
per k; ogni foto viene calibrata solo sui suoi punti (partenze multiple) e si somma l'errore quadratico
di riproiezione. Il k migliore e' quello che rende le foto piu' coerenti tra loro e con il disegno.
Uso: scala_verticale.py <punti.yaml>
"""
import os, sys, numpy as np, yaml
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from PIL import Image
from camera import calibra_punti, q_iniziale

cfg = yaml.safe_load(open(sys.argv[1])); L = cfg['L']; XB = L / 2
def xyz(p, k):
    d, y, z = p; return [XB - d, y, z * k]
def calibra(f, k):
    w, h = Image.open(f['file']).size
    P = [xyz(p, k) for p, q in f['punti']]; Q = [q for p, q in f['punti']]
    ci = f['fotocamera_iniziale']; q0 = q_iniziale(ci['posizione'], ci['bersaglio'], ci['f'])
    best = None
    for daz in (-40, -20, 0, 20, 40):
        for el in (q0[1], 5.0, 15.0, 30.0):
            for fd in (0.5, 1.0, 2.0):
                qs = q0.copy(); qs[0] += daz; qs[1] = el; qs[2] += np.log(fd); qs[6] += np.log(fd)
                _, _, rms = calibra_punti(qs, w, h, P, Q)
                best = rms if best is None or rms < best else best
    return best
print('k     ' + '  '.join(f"{f['nome']:>14s}" for f in cfg['foto']) + '   totale (rms px)')
for k in np.arange(0.9, 1.51, 0.05):
    r = [calibra(f, k) for f in cfg['foto']]
    print(f'{k:4.2f}  ' + '  '.join(f'{x:14.1f}' for x in r) + f'   {np.sqrt(np.mean(np.square(r))):8.1f}', flush=True)
