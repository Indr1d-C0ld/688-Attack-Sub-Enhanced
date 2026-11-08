"""Passo 2: ricerca multi-scala della posizione delle mappe SURFA..G nei teatri reali.

Per ogni teatro si usa ETOPO 2022 a 60" su un'area allargata. Per ogni altezza della mappa
(in gradi di latitudine) l'area viene ricampionata in modo che una cella valga un pixel del
modello ridotto (64x40); la correlazione incrociata normalizzata (NCC, via FFT) trova la
posizione migliore. Si provano 4 orientamenti e 2 rapporti d'aspetto del pixel (quadrato in
miglia, oppure 1.2 come su monitor CRT 4:3). Convenzione: livello alto = fondale meno profondo.
"""
import os, sys, glob, json, numpy as np, xarray as xr, yaml
from scipy.ndimage import zoom, uniform_filter
from scipy.signal import fftconvolve
from scipy.optimize import linear_sum_assignment

root = sys.argv[1]
TW, TH = 64, 40
cfg = yaml.safe_load(open(f'{root}/tools/pipeline/teatri.yaml'))

def load_surf():
    out = {}
    for f in sorted(glob.glob(f'{root}/extracted/bathymetry/SURF?_*.u8')):
        n = os.path.basename(f)[:5]; w, h = map(int, os.path.basename(f)[6:-3].split('x'))
        a = np.frombuffer(open(f, 'rb').read(), np.uint8).reshape(h, w)[:, :318].astype(float)
        out[n] = zoom(a, (TH / a.shape[0], TW / a.shape[1]), order=1)
    return out

def ncc(big, tpl):
    t = tpl - tpl.mean(); t /= np.sqrt((t ** 2).sum()) + 1e-9
    num = fftconvolve(big, t[::-1, ::-1], mode='valid')
    n = tpl.size
    s1 = fftconvolve(big, np.ones_like(tpl), mode='valid')
    s2 = fftconvolve(big ** 2, np.ones_like(tpl), mode='valid')
    var = np.maximum(s2 - s1 ** 2 / n, 1e-9)
    return num / np.sqrt(var)

surf = load_surf()
ORIENT = {'diritta': lambda a: a, 'ruotata180': lambda a: a[::-1, ::-1],
          'capovolta': lambda a: a[::-1], 'specchio': lambda a: a[:, ::-1]}
results = {}
for t in cfg['teatri']:
    z = xr.open_dataset(f'{root}/data/teatri/{t["id"]}/etopo_ampio_60s.nc')['z'].sortby('latitude', ascending=False)
    lat = z.latitude.values; lon = z.longitude.values; zz = np.clip(z.values.astype(float), -3500, 50)
    la0, la1, lo0, lo1 = t['bbox']; clat = np.cos(np.radians((la0 + la1) / 2))
    dlat = abs(lat[1] - lat[0]); dlon = abs(lon[1] - lon[0])
    for hdeg in np.geomspace(0.33, 4.2, 16):          # altezza mappa: ~20..250 nm
        for aspect in (1.0, 1.2):
            cell_lat = hdeg / TH
            cell_lon = cell_lat / clat / aspect * (TW / TH) * (TH / TW) * (318 / 200) / (TW / TH)  # pixel ~ quadrato in nm
            fy = dlat / cell_lat; fx = dlon / cell_lon
            if fy > 1.0 or fx > 1.0: continue          # risoluzione insufficiente
            big = zoom(zz, (fy, fx), order=1)
            if big.shape[0] <= TH or big.shape[1] <= TW: continue
            for n, s in surf.items():
                for on, of in ORIENT.items():
                    c = ncc(big, of(s))
                    # vincolo: il centro della mappa deve cadere nel riquadro del teatro
                    iy, ix = np.indices(c.shape)
                    cy = lat[0] - (iy + TH / 2) * cell_lat; cx = lon[0] + (ix + TW / 2) * cell_lon
                    c = np.where((cy >= la0) & (cy <= la1) & (cx >= lo0) & (cx <= lo1), c, -2)
                    k = np.unravel_index(np.argmax(c), c.shape); v = c[k]
                    key = (n, t['id'])
                    if v > results.get(key, (-9,))[0]:
                        y0 = lat[0] - k[0] * cell_lat; x0 = lon[0] + k[1] * cell_lon
                        results[key] = (float(v), on, float(hdeg), aspect,
                                        [round(y0 - hdeg, 3), round(y0, 3), round(x0, 3), round(x0 + TW * cell_lon, 3)])
names = sorted(surf); tids = [t['id'] for t in cfg['teatri']]
M = np.array([[results[(n, tid)][0] for tid in tids] for n in names])
print('NCC massima (livello alto = basso fondale)')
print('       ' + ' '.join(f'{x:>10s}' for x in tids))
for i, n in enumerate(names):
    print(f'{n:6s} ' + ' '.join(f'{v:10.2f}' for v in M[i]))
r, c = linear_sum_assignment(-M)
out = {}
print('\nAssegnazione ottima (una mappa per teatro):')
for i, j in zip(r, c):
    v, on, h, asp, bb = results[(names[i], tids[j])]
    print(f'{names[i]} -> {tids[j]:10s} NCC {v:.2f}  {on:10s} altezza {h:.2f}° (~{h*60:.0f} nm)  aspetto {asp}  bbox {bb}')
    out[names[i]] = dict(teatro=tids[j], ncc=round(v, 3), orientamento=on, altezza_gradi=round(h, 3),
                         aspetto_pixel=asp, bbox_lat_lon=bb)
json.dump(out, open(f'{root}/data/teatri/associazione_surf.json', 'w'), indent=2, ensure_ascii=False)
