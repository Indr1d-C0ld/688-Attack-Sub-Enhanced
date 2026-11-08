"""Strumenti di lavoro del lotto sottomarini 3 (Typhoon, Foxtrot, Sturgeon, Trafalgar): ritagli con griglia
per annotare i punti a vista. Non fanno parte della pipeline.

Uso: sottomarini3_strumenti.py ritaglio <foto> <uscita.png> x0 y0 x1 y1 [scala] [passo_griglia]
     sottomarini3_strumenti.py punti <foto> <uscita.png> x,y[:etichetta] ...   (croci sui punti, foto intera ridotta)
"""
import sys
from PIL import Image, ImageDraw

def ritaglio(foto, uscita, x0, y0, x1, y1, scala=1.0, passo=50):
    im = Image.open(foto).convert('RGB').crop((x0, y0, x1, y1))
    im = im.resize((round(im.width * scala), round(im.height * scala)), Image.LANCZOS)
    dr = ImageDraw.Draw(im)
    gx = (x0 // passo + 1) * passo
    while gx < x1:
        X = (gx - x0) * scala; dr.line((X, 0, X, 6), fill=(255, 255, 0)); dr.line((X, im.height - 6, X, im.height), fill=(255, 255, 0))
        dr.text((X + 2, 6), str(gx), fill=(255, 255, 0)); gx += passo
    gy = (y0 // passo + 1) * passo
    while gy < y1:
        Y = (gy - y0) * scala; dr.line((0, Y, 6, Y), fill=(0, 255, 255)); dr.line((im.width - 6, Y, im.width, Y), fill=(0, 255, 255))
        dr.text((8, Y - 6), str(gy), fill=(0, 255, 255)); gy += passo
    im.save(uscita)

def punti(foto, uscita, pp, larghezza=1400):
    im = Image.open(foto).convert('RGB'); s = larghezza / im.width
    im = im.resize((larghezza, round(im.height * s)), Image.LANCZOS); dr = ImageDraw.Draw(im)
    for p in pp:
        xy, _, et = p.partition(':'); x, y = [float(v) * s for v in xy.split(',')]
        dr.line((x - 6, y, x + 6, y), fill=(255, 0, 0)); dr.line((x, y - 6, x, y + 6), fill=(255, 0, 0))
        if et: dr.text((x + 4, y + 2), et, fill=(255, 255, 0))
    im.save(uscita)

def scarti(classe, radice='/home/randolph/Claude/melchior-688'):
    """Scarto di ogni punto annotato con le fotocamere di verifica.json (dopo una costruzione)."""
    import json, os, yaml, numpy as np
    sys.path.insert(0, os.path.join(radice, 'tools/navi'))
    import costruisci
    from camera import Camera
    cl = yaml.safe_load(open(classe)); XB = cl['scafo']['L'] / 2
    ver = json.load(open(os.path.join(radice, cl['uscita'], 'verifica.json')))
    for fc in cl['foto']:
        if fc['nome'] not in ver['foto']: continue
        w, h = Image.open(os.path.join(radice, fc['file'])).size
        cam = Camera(ver['foto'][fc['nome']]['fotocamera'], w, h)
        dz = -float(fc.get('affondamento', 0.0))
        P = np.array([costruisci.risolvi(p, XB) for p, q in fc['punti']], float) + [0, 0, dz]
        u, v, _ = cam.proietta(P)
        q = cam.p
        print(f"{fc['nome']}: parametri {np.round(q, 2).tolist()}")
        for (p, qq), uu, vv in zip(fc['punti'], u, v):
            print(f"   {str(p):40s} foto {qq}  modello [{uu:7.1f}, {vv:7.1f}]  scarto {np.hypot(uu - qq[0], vv - qq[1]):5.1f}")

def poligono_dilatato(maschera, k, passo=10, colonne=None):
    """Poligono (bordo alto e basso colonna per colonna) della maschera dilatata di k pixel (negativo: erosa)."""
    import numpy as np
    from scipy import ndimage
    m = np.asarray(Image.open(maschera).convert('L')) > 127
    if k: m = (ndimage.binary_dilation if k > 0 else ndimage.binary_erosion)(m, iterations=abs(k))
    cols = np.flatnonzero(m.any(0))
    xs = list(range(cols[0], cols[-1] + 1, passo)) + [cols[-1]]
    alto = [(int(x), int(np.flatnonzero(m[:, x])[0])) for x in xs if m[:, x].any()]
    basso = [(int(x), int(np.flatnonzero(m[:, x])[-1])) for x in xs[::-1] if m[:, x].any()]
    return [list(p) for p in alto + basso]

def statistiche(foto, maschera):
    """Statistiche di colore (blu = B - (R+G)/2, luminanza) dentro la maschera e nella corona attorno."""
    import numpy as np
    from scipy import ndimage
    m = np.asarray(Image.open(maschera).convert('L')) > 127
    rgb = np.asarray(Image.open(foto).convert('RGB'), float)
    blu = rgb[..., 2] - (rgb[..., 0] + rgb[..., 1]) / 2; lum = rgb @ np.array([0.299, 0.587, 0.114])
    corona = ndimage.binary_dilation(m, iterations=40) & ~ndimage.binary_dilation(m, iterations=10)
    dentro = ndimage.binary_erosion(m, iterations=4)
    for nome, z in (('dentro', dentro), ('corona', corona)):
        print(nome, 'blu percentili 5/25/50/75/95', np.percentile(blu[z], [5, 25, 50, 75, 95]).round(0),
              'lum', np.percentile(lum[z], [5, 25, 50, 75, 95]).round(0))

def mosaico(cartella, uscita, angoli=(0, 30, 60, 90, 120, 150, 180), zoom=2, lati=('dritta', 'sinistra')):
    """Sprite a zoom intero (vicino piu' prossimo) su fondo grigio azzurro, una riga per lato."""
    righe = []
    for lato in lati:
        ims = [Image.open(f'{cartella}/aob_{a:03d}_{lato}.png').convert('RGBA') for a in angoli]
        ims = [im.resize((im.width * zoom, im.height * zoom), Image.NEAREST) for im in ims]
        W = sum(im.width for im in ims) + 8 * len(ims); H = max(im.height for im in ims)
        r = Image.new('RGBA', (W, H + 4), (88, 104, 124, 255)); x = 0
        for im in ims: r.alpha_composite(im, (x, H - im.height)); x += im.width + 8
        righe.append(r)
    out = Image.new('RGBA', (max(r.width for r in righe), sum(r.height for r in righe)), (40, 40, 40, 255)); y = 0
    for r in righe: out.alpha_composite(r, (0, y)); y += r.height
    out.save(uscita)

if __name__ == '__main__':
    if sys.argv[1] == 'mosaico':
        mosaico(sys.argv[2], sys.argv[3], [int(v) for v in sys.argv[4].split(',')] if len(sys.argv) > 4 else (0, 30, 60, 90, 120, 150, 180)); sys.exit()
    if sys.argv[1] == 'poligono':
        print(poligono_dilatato(sys.argv[2], int(sys.argv[3]), int(sys.argv[4]) if len(sys.argv) > 4 else 10)); sys.exit()
    if sys.argv[1] == 'statistiche':
        statistiche(sys.argv[2], sys.argv[3]); sys.exit()
    if sys.argv[1] == 'scarti':
        scarti(sys.argv[2]); sys.exit()
    a = sys.argv
    if a[1] == 'ritaglio':
        v = [int(x) for x in a[4:8]]
        ritaglio(a[2], a[3], *v, float(a[8]) if len(a) > 8 else 1.0, int(a[9]) if len(a) > 9 else 50)
    elif a[1] == 'punti':
        punti(a[2], a[3], a[4:])
