"""Trattamento "digitalizzazione d'epoca" delle foto per il Museo e i briefing.

Pipeline: ritaglio -> scala di grigi -> livelli automatici (percentili 1-99) -> nitidezza leggera ->
ridimensionamento -> quantizzazione su una rampa di grigi con retinatura.
  - variante 'epoca16': 16 grigi della palette originale (indici 144..159 di DAY.PAL), retinatura
    Floyd-Steinberg: l'aspetto delle foto digitalizzate del 1989;
  - variante 'svga64': 64 grigi, retinatura ordinata 4x4 leggera: stessa estetica, piu' dettaglio a 640x480.
"""
import json, os, sys
import numpy as np
from PIL import Image, ImageFilter

BAYER4 = np.array([[0, 8, 2, 10], [12, 4, 14, 6], [3, 11, 1, 9], [15, 7, 13, 5]]) / 16.0 - 0.47

def rampa_originale(root):
    pal = open(f'{root}/extracted/raw/DISK1/DAY.PAL', 'rb').read()
    # banco 144..158: rampa di grigi crescente usata dagli sprite fotografici (159 = trasparente)
    return np.array([pal[3 * i] for i in range(144, 159)], dtype=float)

def livelli(g):
    lo, hi = np.percentile(g, 1), np.percentile(g, 99)
    return np.clip((g - lo) / max(hi - lo, 1) * 255, 0, 255)

def floyd(g, livelli_rgb):
    g = g.copy(); h, w = g.shape; out = np.zeros_like(g, dtype=np.uint8)
    L = np.asarray(livelli_rgb, float)
    for y in range(h):
        for x in range(w):
            v = g[y, x]; k = int(np.abs(L - v).argmin()); out[y, x] = k; e = v - L[k]
            if x + 1 < w: g[y, x + 1] += e * 7 / 16
            if y + 1 < h:
                if x > 0: g[y + 1, x - 1] += e * 3 / 16
                g[y + 1, x] += e * 5 / 16
                if x + 1 < w: g[y + 1, x + 1] += e * 1 / 16
    return out

def ordinato(g, n):
    step = 255 / (n - 1)
    t = BAYER4[np.arange(g.shape[0])[:, None] % 4, np.arange(g.shape[1])[None, :] % 4]
    return np.clip(np.round(g / step + t * 0.6), 0, n - 1).astype(np.uint8)

def tratta(src, crop, larghezza, ramp16):
    im = Image.open(src).convert('L').crop(crop)
    im = im.filter(ImageFilter.UnsharpMask(radius=1.2, percent=60, threshold=2))
    h = round(im.height * larghezza / im.width)
    g = livelli(np.asarray(im.resize((larghezza, h), Image.LANCZOS), float))
    k16 = floyd(g, ramp16)
    e16 = Image.fromarray(k16, 'P'); pal = []
    for v in ramp16: pal += [int(v)] * 3
    e16.putpalette(pal + [0] * (768 - len(pal)))
    k64 = ordinato(g, 64)
    s64 = Image.fromarray(k64, 'P'); pal = []
    for i in range(64): pal += [round(i * 255 / 63)] * 3
    s64.putpalette(pal + [0] * (768 - len(pal)))
    return e16, s64

# ritagli sulle immagini a 960 px (x0, y0, x1, y1), scelti a vista per inquadrare la nave
RITAGLI = {'fianco_sinistro': (30, 170, 910, 470), 'fianco_destro': (0, 60, 960, 330),
           'prua_tre_quarti': (330, 90, 790, 560), 'poppa_tre_quarti': (230, 60, 860, 560),
           'prua': (180, 120, 780, 1020), 'poppa': (180, 120, 780, 1130)}

if __name__ == '__main__':
    root = sys.argv[1]
    d = f'{root}/extracted/prototipo_perry'
    os.makedirs(f'{d}/museo', exist_ok=True)
    ramp = rampa_originale(root)
    for vista, crop in RITAGLI.items():
        e16, s64 = tratta(f'{d}/foto/{vista}.jpg', crop, 400, ramp)
        e16.save(f'{d}/museo/{vista}_epoca16.png'); s64.save(f'{d}/museo/{vista}_svga64.png')
        print(vista, e16.size)
