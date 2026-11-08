"""Georeferenzia le mappe originali SURFA..G nel loro teatro.

L'associazione mappa -> teatro e' quella certa ricavata dai briefing (SCENE*.LZA, token
0B 00 0E nn = indice mappa). Qui si cerca solo posizione, scala, orientamento e aspetto del
pixel con la correlazione di rango (Spearman), su modello 64x40 per maggiore dettaglio.
Uscita: data/teatri/associazione_surf.json e un foglio di confronto visivo.
"""
import os, sys, json, numpy as np, xarray as xr, yaml
from scipy.ndimage import zoom
from scipy.stats import rankdata
from PIL import Image

root = sys.argv[1]
MAPPA = {'SURFA': 'otranto', 'SURFB': 'baltico', 'SURFC': 'barents', 'SURFD': 'gibilterra',
         'SURFE': 'kunashir', 'SURFF': 'celtica', 'SURFG': 'faroe'}
TW, TH = 64, 40
cfg = {t['id']: t for t in yaml.safe_load(open(f'{root}/tools/pipeline/teatri.yaml'))['teatri']}

def zrank(a):
    r = rankdata(a, method='average', axis=-1); r -= r.mean(-1, keepdims=True)
    return r / (np.linalg.norm(r, axis=-1, keepdims=True) + 1e-9)

def load(n):
    import glob
    f = glob.glob(f'{root}/extracted/bathymetry/{n}_*.u8')[0]
    w, h = map(int, os.path.basename(f)[6:-3].split('x'))
    return np.frombuffer(open(f, 'rb').read(), np.uint8).reshape(h, w)[:, :318].astype(float)

out = {}; tiles = []
for n, tid in MAPPA.items():
    t = cfg[tid]; full = load(n)
    small = zoom(full, (TH / 200, TW / 318), order=1)
    ori = {'diritta': small, 'ruotata180': small[::-1, ::-1], 'capovolta': small[::-1], 'specchio': small[:, ::-1]}
    T = zrank(np.array([o.ravel() for o in ori.values()])); onames = list(ori)
    z = xr.open_dataset(f'{root}/data/teatri/{tid}/etopo_ampio_60s.nc')['z'].sortby('latitude', ascending=False)
    lat = z.latitude.values; lon = z.longitude.values; zz = np.minimum(z.values.astype(float), 0.0)
    la0, la1, lo0, lo1 = t['bbox']; clat = np.cos(np.radians((la0 + la1) / 2))
    dlat = abs(lat[1] - lat[0]); dlon = abs(lon[1] - lon[0])
    cands = []
    for hdeg in np.geomspace(0.33, 4.2, 22):
        for aspect in (1.0, 1.2):
            cl = hdeg / TH; cw = (hdeg * 60 * (318 / 200) / aspect / TW) / 60 / clat
            fy, fx = dlat / cl, dlon / cw
            if fy > 3.0 or fx > 3.0: continue
            big = zoom(zz, (fy, fx), order=1)
            if big.shape[0] <= TH or big.shape[1] <= TW: continue
            win = np.lib.stride_tricks.sliding_window_view(big, (TH, TW))
            ny, nx = win.shape[:2]
            cy = lat[0] - (np.arange(ny) + TH / 2) * cl; cx = lon[0] + (np.arange(nx) + TW / 2) * cw
            m = 0.5   # il centro puo' uscire dal riquadro del dossier di mezzo grado al massimo
            ys = np.flatnonzero((cy >= la0 - m) & (cy <= la1 + m))[::2]; xs = np.flatnonzero((cx >= lo0 - m) & (cx <= lo1 + m))[::2]
            if len(ys) == 0 or len(xs) == 0: continue
            W = win[np.ix_(ys, xs)].reshape(-1, TH * TW)
            keep = np.flatnonzero(W.std(1) > 1.0)
            if len(keep) == 0: continue
            S = zrank(W[keep]) @ T.T
            k, j = np.unravel_index(np.argmax(S), S.shape)
            iy, ix = np.unravel_index(keep[k], (len(ys), len(xs)))
            y0 = lat[0] - ys[iy] * cl; x0 = lon[0] + xs[ix] * cw
            cands.append((float(S[k, j]), onames[j], float(hdeg), aspect,
                          [round(float(y0 - hdeg), 3), round(float(y0), 3), round(float(x0), 3), round(float(x0 + TW * cw), 3)]))
    cands.sort(reverse=True)
    v, on, h, asp, bb = cands[0]
    print(f'{n} {tid:10s} rho {v:.3f} {on:10s} ~{h*60:.0f}x{h*60*318/200/asp:.0f} nm aspetto {asp} bbox {bb}')
    for c in cands[1:3]: print(f'      alternativa rho {c[0]:.3f} {c[1]:10s} ~{c[2]*60:.0f} nm bbox {c[4]}')
    out[n] = dict(teatro=tid, indice_mappa='ABCDEFG'.index(n[-1]), spearman=round(v, 3), orientamento=on,
                  altezza_nm=round(h * 60), larghezza_nm=round(h * 60 * 318 / 200 / asp), aspetto_pixel=asp,
                  bbox_lat_lon=bb)
    # tassello di verifica: mappa originale (orientata) accanto a ETOPO sul riquadro trovato
    sel = z.sel(latitude=slice(bb[1], bb[0]), longitude=slice(bb[2], bb[3])).values.astype(float)
    if sel.size:
        e = zoom(np.minimum(sel, 0), (200 / sel.shape[0], 318 / sel.shape[1]), order=1)
        e = (rankdata(e).reshape(e.shape) / e.size * 255).astype(np.uint8)
        o = {'diritta': full, 'ruotata180': full[::-1, ::-1], 'capovolta': full[::-1], 'specchio': full[:, ::-1]}[on]
        tiles.append((Image.fromarray((o / 15 * 255).astype(np.uint8)), Image.fromarray(e[:200, :318])))
json.dump(dict(fonte='indice mappa dai briefing SCENE*.LZA (token 0B 00 0E nn); posizione da Spearman su ETOPO 2022',
               mappe=out), open(f'{root}/data/teatri/associazione_surf.json', 'w'), indent=2, ensure_ascii=False)
c = Image.new('L', (2 * 324, len(tiles) * 204), 40)
for i, (a, b) in enumerate(tiles): c.paste(a, (0, i * 204)); c.paste(b, (324, i * 204))
c.save(f'{root}/extracted/georef_surf_verifica.png')
