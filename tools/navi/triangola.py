"""Triangolazione da piu' foto (aggiustamento a fascio semplificato).

Stima insieme le fotocamere delle foto e le posizioni 3D di elementi della nave annotati in piu'
foto. Ancore: lunghezza fuori tutto (x del dritto in coperta = +L/2, x dello specchio = -L/2),
galleggiamento z = 0 per i punti al galleggiamento, y = 0 per gli elementi sull'asse; facoltativo per
ogni foto `orizzonte_punti`, pixel sulla linea dell'orizzonte marino (vincola quota ed elevazione).
Per ogni foto, facoltativi: `prior_camera` {elevazione (gradi, predefinita quella della stima iniziale),
sigma_el, distanza (m), sigma_logD} vincolo debole sulla fotocamera, utile quando i punti sono quasi tutti
sul piano di simmetria; `limiti_fotocamera`, `verticali`, `assetto_nave` e `aspetto_pixel` come nelle classi
(docs/06_kit_navi.md).
Elementi simmetrici: `simmetrico_di: altro` (es. le due estremita' delle alette di plancia) impone la stessa x
e quota dell'altro elemento e la y opposta; l'elemento non ha incognite proprie.
Le incognite hanno un vincolo debole (sigma in metri) verso la stima iniziale.
Le fotocamere stimate vengono salvate in JSON (`uscita_fotocamere`, predefinito <file>_fotocamere.json), nel
formato `fotocamera_iniziale` delle classi e con i parametri completi.
Stampa gli scarti di ogni punto (i punti identificati male saltano all'occhio) e avvisa quando una
fotocamera finisce su un limite fisico.
Uso: triangola.py <triangolazione.yaml> [radice del progetto]   (i file delle foto sono relativi alla radice)
"""
import os, sys, numpy as np, yaml
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from PIL import Image
from scipy.optimize import least_squares
import json
from camera import (da_q, q_iniziale, calibra_punti, scarto_orizzonte, scarto_prior, scarto_verticali, limiti_foto,
                    al_limite)

RADICE = sys.argv[2] if len(sys.argv) > 2 else os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
cfg = yaml.safe_load(open(sys.argv[1])); L = cfg['L']; XB = L / 2
el = cfg['elementi']                       # nome -> {xyz: [d, y, z], liberi: 'xz'|..., sigma: m}
nomi = list(el)
# vettore delle incognite: per ogni elemento, le coordinate libere
idx = []
for n in nomi:
    if 'simmetrico_di' in el[n]: continue            # nessuna incognita propria: segue il compagno
    for c in el[n].get('liberi', ''):
        idx.append((n, 'xyz'.index(c)))
def base(n):
    if 'simmetrico_di' in el[n]:
        p = base(el[n]['simmetrico_di']); return np.array([p[0], -p[1], p[2]])
    d, y, z = el[n]['xyz']; return np.array([XB - d, y, z], float)
x0_el = np.array([base(n)[c] for n, c in idx])
foto = cfg['foto']
# aspetto_pixel: coordinate verticali riscalate come nelle classi (la foto ricampionata in altezza)
def _scala(f):
    k = float(f.get('aspetto_pixel', 1.0))
    if k == 1.0: return f
    f = dict(f); uv = lambda p: [p[0], p[1] * k]
    f['osservazioni'] = {n: uv(q) for n, q in f['osservazioni'].items()}
    if f.get('orizzonte_punti'): f['orizzonte_punti'] = [uv(p) for p in f['orizzonte_punti']]
    if f.get('verticali'): f['verticali'] = [[uv(a), uv(b)] for a, b in f['verticali']]
    return f
foto = [_scala(f) for f in foto]
dims = []
for f in foto:
    w, h = Image.open(os.path.join(RADICE, f['file'])).size
    dims.append((w, round(h * float(f.get('aspetto_pixel', 1.0)))))
lims = [limiti_foto(f) for f in foto]
# fotocamere iniziali: calibrazione sui soli punti, con partenze multiple
q0s = []; priors = []
for f, (w, h), lim in zip(foto, dims, lims):
    P = [base(n) for n in f['osservazioni']]; Q = list(f['osservazioni'].values())
    ci = f['fotocamera_iniziale']; q0 = q_iniziale(ci['posizione'], ci['bersaglio'], ci['f'])
    pr = f.get('prior_camera')
    if pr:      # prior senza valori espliciti: verso la stima iniziale
        pr = dict(pr)
        if 'sigma_el' in pr: pr.setdefault('elevazione', float(q0[1]))
        if 'sigma_logD' in pr: pr.setdefault('distanza', float(np.exp(q0[2])))
    priors.append(pr)
    best = None
    for daz in (-40, -20, 0, 20, 40):
        for e in (q0[1], 5.0, 15.0, 30.0):
            for fd in (0.5, 1.0, 2.0):
                qs = q0.copy(); qs[0] += daz; qs[1] = e; qs[2] += np.log(fd); qs[6] += np.log(fd)
                qs = np.clip(qs, lim[:, 0], lim[:, 1])   # le partenze multiple restano nei limiti fisici
                qq, _, rms = calibra_punti(qs, w, h, P, Q, orizzonte=f.get('orizzonte_punti'), limiti=lim, prior=pr,
                                           verticali=f.get('verticali'), assetto=f.get('assetto_nave'))
                if best is None or rms < best[1]: best = (qq, rms)
    q0s.append(best[0])
def unpack(v):
    qs = [v[7 * i:7 * i + 7] for i in range(len(foto))]
    pts = {n: base(n) for n in nomi}
    for k, (n, c) in enumerate(idx): pts[n][c] = v[7 * len(foto) + k]
    for n in nomi:                                   # simmetrici: dalla posizione stimata del compagno
        if 'simmetrico_di' in el[n]:
            p = pts[el[n]['simmetrico_di']]; pts[n] = np.array([p[0], -p[1], p[2]])
    return qs, pts
def res(v):
    qs, pts = unpack(v); r = []
    for f, q, (w, h), pr in zip(foto, qs, dims, priors):
        cam = da_q(q, w, h)
        for n, (u, vv) in f['osservazioni'].items():
            pu, pv, _ = cam.proietta(pts[n]); r += [pu - u, pv - vv]
        r += list(scarto_orizzonte(cam, f.get('orizzonte_punti'), f.get('assetto_nave')))
        r += list(scarto_verticali(cam, f.get('verticali')))
        r += list(scarto_prior(q, pr))
    # vincoli deboli verso la stima iniziale (convertiti in pixel equivalenti: 1 sigma = 5 px)
    for k, (n, c) in enumerate(idx):
        r.append(5.0 * (v[7 * len(foto) + k] - x0_el[k]) / el[n].get('sigma', float(os.environ.get('SIGMA', 3.0))))
    return np.array(r)
v0 = np.concatenate(q0s + [x0_el])
lb = np.concatenate([l[:, 0] for l in lims] + [np.full(len(idx), -1e4)])
ub = np.concatenate([l[:, 1] for l in lims] + [np.full(len(idx), 1e4)])
sol = least_squares(res, v0, bounds=(lb, ub), loss='soft_l1', f_scale=5.0, x_scale='jac')
qs, pts = unpack(sol.x)
print('Errore per foto (rms px) e scarti dei singoli punti (du, dv):')
for f, q, (w, h), lim in zip(foto, qs, dims, lims):
    cam = da_q(q, w, h); e = []; righe = []
    for n, (u, vv) in f['osservazioni'].items():
        pu, pv, _ = cam.proietta(pts[n]); e.append(np.hypot(pu - u, pv - vv))
        righe.append(f"      {n:22s} {pu - u:+7.1f} {pv - vv:+7.1f}" + ('   <-- da controllare' if e[-1] > 3 * np.median(e) + 2 else ''))
    eo = scarto_orizzonte(cam, f.get('orizzonte_punti'))
    extra = f"   orizzonte {np.sqrt(np.mean(eo ** 2)):.1f}" if eo.size else ''
    print(f"  {f['nome']:16s} {np.sqrt(np.mean(np.square(e))):6.1f}   (max {max(e):.1f})   fotocamera {np.round(cam.C, 0)}"
          f"   elevazione {q[1]:.1f} gradi{extra}")
    print('\n'.join(righe))
    print(f"    fotocamera_iniziale: {{posizione: {[round(float(c), 1) for c in cam.C]}, "
          f"bersaglio: {[round(float(q[3]), 1), 0.0, round(float(q[4]), 1)]}, f: {round(float(np.exp(q[6])))}}}")
    lr = al_limite(q, lim)
    if lr: print(f"    AVVISO: {', '.join(lr)} (calibrazione forzata? vedi limiti_fotocamera / prior_camera)")
print('Elementi triangolati (distanza dalla prua, y, quota) [stima iniziale]:')
for n in nomi:
    p = pts[n]; b = base(n)
    print(f"  {n:22s} d={XB - p[0]:6.1f}  y={p[1]:5.1f}  z={p[2]:5.1f}   [d={XB - b[0]:5.1f} z={b[2]:5.1f}]")
# fotocamere stimate, riusabili nelle classi (fotocamera_iniziale) o per proiettare il modello sulle foto
uscita = cfg.get('uscita_fotocamere') or os.path.splitext(sys.argv[1])[0] + '_fotocamere.json'
json.dump({f['nome']: dict(fotocamera_iniziale=dict(posizione=[round(float(c), 2) for c in da_q(q, w, h).C],
                                                    bersaglio=[round(float(q[3]), 2), 0.0, round(float(q[4]), 2)],
                                                    f=round(float(np.exp(q[6])), 1)),
                           parametri=[round(float(x), 6) for x in q], larghezza=w, altezza=h)
           for f, q, (w, h) in zip(foto, qs, dims)}, open(uscita, 'w'), indent=1, ensure_ascii=False)
print('fotocamere salvate in', uscita)
