"""Strumento di lavoro del lotto sottomarini 2 (Alfa, Akula, Oscar II, Charlie II); vedi i RAPPORTO.md.
Uso: sottomarini2_poligono.py <classe.yaml> <foto> <parti,...> <dilata_px> [passo] [tutti]   (con `tutti`: lista di poligoni, uno per tratto staccato)
Contorno (alto e basso per colonna) della sagoma del modello proiettata con la fotocamera di verifica.json,
allargato di dilata_px, come poligono YAML su una riga (per `scafo`/`includi` di maschera_manuale.py)."""
import json, os, sys
R = '/home/randolph/Claude/melchior-688'; sys.path.insert(0, R + '/tools/navi')
import numpy as np, yaml
from PIL import Image
from scipy import ndimage
import costruisci, raster
from camera import Camera
cl = yaml.safe_load(open(sys.argv[1])); mesh = costruisci.costruisci_mesh(cl, os.path.dirname(os.path.abspath(sys.argv[1])))
fc = [f for f in cl['foto'] if f['nome'] == sys.argv[2]][0]
parti = set(sys.argv[3].split(',')); k = int(sys.argv[4]); passo = int(sys.argv[5]) if len(sys.argv) > 5 else 8
w, h = Image.open(f"{R}/{fc['file']}").size
cam = Camera(json.load(open(f"{R}/{cl['uscita']}/verifica.json"))['foto'][fc['nome']]['fotocamera'], w, h)
tri = mesh.tri[np.array([p in parti for p in mesh.parte])].copy(); tri[..., 2] -= float(fc.get('affondamento', 0.0))
sag = raster.sagoma(cam, tri, 1.0)
if k > 0: sag = ndimage.binary_dilation(sag, iterations=k)
cols = np.flatnonzero(sag.any(0))
# tratti di colonne contigue (sagome staccate: timone e pod separati dallo scafo): un poligono per tratto
tratti = np.split(cols, np.flatnonzero(np.diff(cols) > 1) + 1)
poli = []
for t in tratti:
    xs = list(range(t[0], t[-1] + 1, passo)) + [t[-1]]
    alto, basso = [], []
    for x in xs:
        ys = np.flatnonzero(sag[:, x]); alto.append([int(x), int(ys[0])]); basso.append([int(x), int(ys[-1])])
    poli.append(alto + basso[::-1])
if len(sys.argv) > 6: print(json.dumps(poli).replace(' ', ''))
else: print(json.dumps(max(poli, key=len)).replace(' ', ''))
