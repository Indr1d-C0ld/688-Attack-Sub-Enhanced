"""Maschera di una foto d'aereo dalla sagoma del modello calibrato e dal colore del fondo (dalle classi dell'ASW aereo).

Uso: maschera_colore_modello.py <classe.yaml> <nome della foto> <uscita.png> [--soglia-blu 20] [--dilata 24]
     [--erodi 0] [--solo-sagoma] [--escludi x0,y0,x1,y1 ...] [--includi-dischi]

La fotocamera viene da extracted/<classe>/verifica.json (prima costruzione, anche solo coi punti). La sagoma del
modello, senza i dischi dei rotori e delle eliche (materiali 'traliccio' e 'retinato', pale congelate dall'otturatore
nelle foto), allargata di --dilata pixel, fa da inviluppo; dentro, e' aereo cio' che non e' blu:
B - (R + G) / 2 < --soglia-blu (cielo e mare sono blu; l'aereo grigio, bianco, verde o mimetico no). Con --solo-sagoma
la maschera e' la sagoma ristretta di --erodi pixel: per le foto di museo con il prato e gli altri aerei sul fondo,
dove il colore non separa. Scrive anche <uscita>_controllo.png (aereo in rosso sulla foto).
"""
import argparse, json, os, sys
import numpy as np, yaml
from PIL import Image
from scipy import ndimage
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import costruisci, raster
from camera import Camera

RADICE = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('classe'); ap.add_argument('foto'); ap.add_argument('uscita')
    ap.add_argument('--soglia-blu', type=float, default=20.0); ap.add_argument('--dilata', type=int, default=24)
    ap.add_argument('--erodi', type=int, default=0); ap.add_argument('--solo-sagoma', action='store_true')
    ap.add_argument('--escludi', nargs='*', default=[]); ap.add_argument('--includi-dischi', action='store_true')
    a = ap.parse_args()
    cl = yaml.safe_load(open(a.classe))
    mesh = costruisci.costruisci_mesh(cl, os.path.dirname(os.path.abspath(a.classe)))
    fc = [f for f in cl['foto'] if f['nome'] == a.foto][0]
    im = Image.open(os.path.join(RADICE, fc['file'])).convert('RGB')
    w, h = im.size
    ver = json.load(open(os.path.join(RADICE, cl['uscita'], 'verifica.json')))
    cam = Camera(ver['foto'][a.foto]['fotocamera'], w, h)
    tieni = np.array([a.includi_dischi or m not in ('traliccio', 'retinato') for m in mesh.mat])
    sag = raster.sagoma(cam, mesh.tri[tieni], 1.0)
    if a.solo_sagoma:
        m = ndimage.binary_erosion(sag, iterations=a.erodi) if a.erodi > 0 else sag
    else:
        rgb = np.asarray(im, float)
        blu = rgb[..., 2] - 0.5 * (rgb[..., 0] + rgb[..., 1])
        blu = ndimage.uniform_filter(blu, 3)
        inv = ndimage.binary_dilation(sag, iterations=a.dilata)
        m = inv & (blu < a.soglia_blu)
        m = ndimage.binary_opening(m, np.ones((2, 2)))
        lab, n = ndimage.label(m)
        if n:   # solo i pezzi che toccano la sagoma del modello
            vicini = np.unique(lab[m & ndimage.binary_dilation(sag, iterations=3)])
            m = np.isin(lab, vicini[vicini > 0])
        m = ndimage.binary_fill_holes(m)
    for r in a.escludi:
        x0, y0, x1, y1 = map(int, r.split(',')); m[y0:y1, x0:x1] = False
    Image.fromarray((m * 255).astype(np.uint8)).save(a.uscita)
    ctl = np.asarray(im).copy(); ctl[m] = (0.5 * ctl[m] + [120, 0, 0]).astype(np.uint8)
    Image.fromarray(ctl).save(a.uscita.replace('.png', '_controllo.png'))
    print(f'{a.uscita}: {int(m.sum())} pixel (sagoma {int(sag.sum())})')


if __name__ == '__main__':
    main()
