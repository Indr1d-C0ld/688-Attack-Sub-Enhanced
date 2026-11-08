"""Poligoni dei dischi delle eliche proiettati su una foto calibrata, da usare come `escludi` nelle maschere
(le pale ferme o sfocate nelle foto non devono dipingere i dischi del modello).

Uso: aerei_eliche_maschera.py <classe.yaml> <nome foto> [dilatazione=1.08]
Usa la fotocamera di <uscita>/verifica.json; stampa un poligono (inviluppo convesso) per ogni disco.
I dischi visibili dalla fotocamera per meno del 45% (nascosti da fusoliera, gondole o altri dischi) vengono saltati:
escluderli toglierebbe tessitura alle parti che li coprono.
"""
import sys, os, json, numpy as np, yaml
sys.path.insert(0, '/home/randolph/Claude/melchior-688/tools/navi')
from PIL import Image
from scipy.spatial import ConvexHull
import costruisci, raster
from camera import Camera
R = '/home/randolph/Claude/melchior-688'
cl = yaml.safe_load(open(sys.argv[1])); nome = sys.argv[2]; k = float(sys.argv[3]) if len(sys.argv) > 3 else 1.08
mesh = costruisci.costruisci_mesh(cl, os.path.dirname(os.path.abspath(sys.argv[1])))
fc = [f for f in cl['foto'] if f['nome'] == nome][0]
w, h = Image.open(f"{R}/{fc['file']}").size
cam = Camera(json.load(open(f"{R}/{cl['uscita']}/verifica.json"))['foto'][nome]['fotocamera'], w, h)
sel = np.array(['elica' in p for p in mesh.parte])
T = mesh.tri[sel]; parti = np.array(mesh.parte)[sel]
c = T.mean(1)
# visibilita': triangoli che vincono lo z-buffer dalla fotocamera della foto (meta' risoluzione)
from camera import Camera as _C
cam_d = _C(np.r_[cam.p[:6], cam.p[6] * 0.5], w // 2, h // 2)
tid, _, _, _ = raster.gbuffer(cam_d, mesh.tri)
vis_tri = np.bincount(tid[tid >= 0].ravel(), minlength=len(mesh.tri))
idx_sel = np.flatnonzero(sel)
gruppi = {}
for i in range(len(T)):
    gruppi.setdefault((parti[i], np.sign(round(c[i, 1], 3))), []).append(i)
for (p, s), idx in sorted(gruppi.items()):
    soli = [i for i in idx if mesh.mat[idx_sel[i]] != 'sovrastruttura']        # dischi (non le ogive)
    tutti = raster.sagoma(cam_d, T[soli], 1.0).sum()
    visti = vis_tri[idx_sel[soli]].sum()
    if tutti and visti < 0.45 * tutti:
        print(f"# {p} y{'+' if s > 0 else '-'}: visibile per il {100 * visti / tutti:.0f}%, saltato"); continue
    V = T[idx].reshape(-1, 3); u, v, z = cam.proietta(V)
    P = np.stack([u, v], 1); hull = P[ConvexHull(P).vertices]; m = hull.mean(0); hull = m + (hull - m) * k
    print(f"# {p} y{'+' if s > 0 else '-'}")
    print('- ' + json.dumps([[int(round(a)), int(round(b))] for a, b in hull[::max(1, len(hull) // 24)]]))
