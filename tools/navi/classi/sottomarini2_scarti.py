"""Strumento di lavoro del lotto sottomarini 2 (Alfa, Akula, Oscar II, Charlie II); vedi i RAPPORTO.md.
Uso: sottomarini2_scarti.py <classe.yaml> [foto...]: scarti dei singoli punti con le fotocamere di verifica.json e
immagini di controllo (verde = annotato, rosso = proiettato, giallo = contorno del modello) in $USCITA_SCARTI/scarti_<foto>.png."""
import json, os, sys
R = '/home/randolph/Claude/melchior-688'
sys.path.insert(0, R + '/tools/navi')
import numpy as np, yaml
from PIL import Image, ImageDraw
import costruisci, raster
from camera import Camera
S = os.environ.get('USCITA_SCARTI', '.')   # cartella delle immagini di controllo
cl = yaml.safe_load(open(sys.argv[1])); mesh = costruisci.costruisci_mesh(cl, os.path.dirname(os.path.abspath(sys.argv[1])))
ver = json.load(open(f"{R}/{cl['uscita']}/verifica.json")); XB = cl['scafo']['L'] / 2
for fc in cl['foto']:
    if len(sys.argv) > 2 and fc['nome'] not in sys.argv[2:]: continue
    im = Image.open(f"{R}/{fc['file']}").convert('RGB'); w, h = im.size
    cam = Camera(ver['foto'][fc['nome']]['fotocamera'], w, h)
    dz = -float(fc.get('affondamento', 0.0))
    tri = mesh.tri.copy(); tri[..., 2] += dz
    sag = raster.sagoma(cam, tri, 1.0)
    a = np.asarray(im).copy(); b = sag ^ np.roll(sag, 1, 0) | sag ^ np.roll(sag, 1, 1); a[b] = [255, 220, 0]
    im = Image.fromarray(a); d = ImageDraw.Draw(im)
    print(fc['nome'], 'posizione fotocamera', np.round(cam.C, 1))
    for P, q in fc['punti']:
        X = np.array([costruisci.risolvi(P[0], XB), P[1], P[2] + dz], float)
        u, v, _ = cam.proietta(X[None])
        print(f"   {str(P):40s} annotato {q}  proiettato [{u[0]:.0f}, {v[0]:.0f}]  scarto {u[0]-q[0]:+.0f} {v[0]-q[1]:+.0f}")
        r = max(3, w // 300)
        d.ellipse((q[0]-r, q[1]-r, q[0]+r, q[1]+r), outline=(0, 255, 0)); d.line((q[0], q[1], u[0], v[0]), fill=(255, 0, 0))
    im.save(f"{S}/scarti_{fc['nome']}.png")
