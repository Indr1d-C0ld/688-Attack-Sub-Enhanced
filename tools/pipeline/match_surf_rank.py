"""Passo 3: ricerca multi-scala con correlazione di rango (Spearman), invariante a qualsiasi
corrispondenza monotona livello -> profondita'. Modello ridotto 32x20; finestre candidate su
tutte le scale/posizioni con centro nel riquadro del teatro; Spearman = Pearson dei ranghi,
calcolato per tutte le finestre e tutte le mappe/orientamenti con un prodotto di matrici.
"""
import os, sys, glob, json, numpy as np, xarray as xr, yaml
from scipy.ndimage import zoom
from scipy.stats import rankdata
from scipy.optimize import linear_sum_assignment

root = sys.argv[1]
TW, TH = 32, 20
cfg = yaml.safe_load(open(f'{root}/tools/pipeline/teatri.yaml'))

def zrank(a):
    r = rankdata(a, method='average', axis=-1)
    r -= r.mean(-1, keepdims=True)
    return r / (np.linalg.norm(r, axis=-1, keepdims=True) + 1e-9)

names, tpls, labels = [], [], []
for f in sorted(glob.glob(f'{root}/extracted/bathymetry/SURF?_*.u8')):
    n = os.path.basename(f)[:5]; w, h = map(int, os.path.basename(f)[6:-3].split('x'))
    a = np.frombuffer(open(f, 'rb').read(), np.uint8).reshape(h, w)[:, :318].astype(float)
    a = zoom(a, (TH / 200, TW / 318), order=1)
    for on, o in (('diritta', a), ('ruotata180', a[::-1, ::-1]), ('capovolta', a[::-1]), ('specchio', a[:, ::-1])):
        tpls.append(o.ravel()); labels.append((n, on))
    names.append(n)
T = zrank(np.array(tpls))                       # (28, 640)

best = {}
for t in cfg['teatri']:
    z = xr.open_dataset(f'{root}/data/teatri/{t["id"]}/etopo_ampio_60s.nc')['z'].sortby('latitude', ascending=False)
    lat = z.latitude.values; lon = z.longitude.values; zz = np.minimum(z.values.astype(float), 0.0)
    la0, la1, lo0, lo1 = t['bbox']; clat = np.cos(np.radians((la0 + la1) / 2))
    dlat = abs(lat[1] - lat[0]); dlon = abs(lon[1] - lon[0])
    for hdeg in np.geomspace(0.33, 4.2, 22):
        for aspect in (1.0, 1.2):
            cl = hdeg / TH; cw = (hdeg * 60 * (318 / 200) / aspect / TW) / 60 / clat
            fy, fx = dlat / cl, dlon / cw
            if fy > 2.0 or fx > 2.0: continue
            big = zoom(zz, (fy, fx), order=1)
            ys = np.arange(0, big.shape[0] - TH, 1); xs = np.arange(0, big.shape[1] - TW, 1)
            cy = lat[0] - (ys + TH / 2) * cl; cx = lon[0] + (xs + TW / 2) * cw
            ys = ys[(cy >= la0) & (cy <= la1)]; xs = xs[(cx >= lo0) & (cx <= lo1)]
            if len(ys) == 0 or len(xs) == 0: continue
            win = np.lib.stride_tricks.sliding_window_view(big, (TH, TW))[np.ix_(ys, xs)]
            W = win.reshape(-1, TH * TW)
            keep = W.std(1) > 1.0                   # scarta finestre piatte (tutta terra o fondo uniforme)
            if not keep.any(): continue
            idx = np.flatnonzero(keep)
            S = zrank(W[keep]) @ T.T                # (finestre, 28)
            for j, (n, on) in enumerate(labels):
                k = np.argmax(S[:, j]); v = S[k, j]
                key = (n, t['id'])
                if v > best.get(key, (-9,))[0]:
                    iy, ix = np.unravel_index(idx[k], (len(ys), len(xs)))
                    y0 = lat[0] - ys[iy] * cl; x0 = lon[0] + xs[ix] * cw
                    best[key] = (float(v), on, float(hdeg), aspect,
                                 [round(float(y0 - hdeg), 3), round(float(y0), 3), round(float(x0), 3), round(float(x0 + TW * cw), 3)])
tids = [t['id'] for t in cfg['teatri']]
M = np.array([[best[(n, tid)][0] for tid in tids] for n in names])
print('Spearman massimo (positivo = livello alto dove il fondale e\' meno profondo)')
print('       ' + ' '.join(f'{x:>10s}' for x in tids))
for i, n in enumerate(names):
    print(f'{n:6s} ' + ' '.join(f'{v:10.2f}' for v in M[i]) + f'   margine {np.sort(M[i])[-1]-np.sort(M[i])[-2]:.2f}')
r, c = linear_sum_assignment(-M)
out = {}
print('\nAssegnazione ottima:')
for i, j in zip(r, c):
    v, on, h, asp, bb = best[(names[i], tids[j])]
    print(f'{names[i]} -> {tids[j]:10s} rho {v:.2f}  {on:10s} ~{h*60:.0f} nm  aspetto {asp}  bbox {bb}')
    out[names[i]] = dict(teatro=tids[j], spearman=round(v, 3), orientamento=on, altezza_nm=round(h * 60),
                         aspetto_pixel=asp, bbox_lat_lon=bb)
json.dump(dict(metodo='spearman multi-scala 32x20', matrice={n: dict(zip(tids, map(float, M[i]))) for i, n in enumerate(names)},
               assegnazione=out), open(f'{root}/data/teatri/associazione_surf.json', 'w'), indent=2, ensure_ascii=False)
