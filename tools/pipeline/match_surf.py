"""Associa le mappe batimetriche originali SURFA..G ai teatri reali (ETOPO 2022).

Passo 1 (questo script): correlazione di rango (Spearman) tra i livelli 0..15 della mappa
originale e la quota ETOPO ricampionata a 320x200 sul riquadro del teatro, provando
l'orientamento verticale (immagine capovolta o no) e speculare orizzontale.
Produce anche un foglio di confronto visivo.
"""
import os, sys, glob, numpy as np, xarray as xr, yaml
from PIL import Image
from scipy.stats import spearmanr

root = sys.argv[1]
cfg = yaml.safe_load(open(f'{root}/tools/pipeline/teatri.yaml'))
surf = {}
for f in sorted(glob.glob(f'{root}/extracted/bathymetry/SURF?_*.u8')):
    n = os.path.basename(f)[:5]; w, h = map(int, os.path.basename(f)[6:-3].split('x'))
    surf[n] = np.frombuffer(open(f, 'rb').read(), np.uint8).reshape(h, w)[:, :318].astype(float)

def etopo_img(tid, w=318, h=200):
    z = xr.open_dataset(f'{root}/data/teatri/{tid}/etopo15s.nc')['z']
    z = z.sortby('latitude', ascending=False)   # nord in alto
    a = z.values.astype(float)
    yi = np.linspace(0, a.shape[0] - 1, h).round().astype(int)
    xi = np.linspace(0, a.shape[1] - 1, w).round().astype(int)
    return a[np.ix_(yi, xi)]

et = {t['id']: etopo_img(t['id']) for t in cfg['teatri']}
print('rho Spearman (|max| su orientamenti); segno: + = livello alto dove il fondale e\' meno profondo')
print('        ' + ' '.join(f'{k:>11s}' for k in et))
best = {}
for n, s in surf.items():
    row = []
    for tid, z in et.items():
        res = []
        for name, v in (('diritta', s), ('capovolta', s[::-1]), ('specchio', s[:, ::-1]), ('capov+spec', s[::-1, ::-1])):
            r = spearmanr(v.ravel(), z.ravel())[0]
            res.append((abs(r), r, name))
        m = max(res); row.append(m)
        best.setdefault(n, []).append((m[0], tid, m[1], m[2]))
    print(f'{n:7s} ' + ' '.join(f'{r[1]:+.2f}{r[2][:2]:>5s}' for r in row))
print()
for n, l in best.items():
    l.sort(reverse=True)
    print(n, '->', l[0][1], f'(rho {l[0][2]:+.2f}, {l[0][3]})', ' seconda:', l[1][1], f'{l[1][2]:+.2f}')

# foglio visivo: riga 1 mappe originali, riga 2 teatri reali (scala di grigi sulla profondita', terra in ocra)
def render_et(z):
    d = np.clip(-z, 0, 3000) / 3000
    rgb = np.stack([20 + 200 * (1 - d), 40 + 190 * (1 - d), 90 + 160 * (1 - d)], -1)
    rgb[z >= 0] = (190, 160, 100)
    return Image.fromarray(rgb.astype(np.uint8))
def render_s(s):
    v = (s / 15 * 230 + 20).astype(np.uint8)
    return Image.fromarray(v).convert('RGB')
c = Image.new('RGB', (7 * 324, 2 * 220), (30, 30, 30))
for i, (n, s) in enumerate(surf.items()): c.paste(render_s(s), (i * 324, 0))
for i, (tid, z) in enumerate(et.items()): c.paste(render_et(z), (i * 324, 220))
c.save(f'{root}/extracted/confronto_surf_teatri.png')
