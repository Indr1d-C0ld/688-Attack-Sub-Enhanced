"""Maschere per tinta delle foto di elicotteri in livrea colorata su fondo misto (cielo, erba, cemento), caso non
coperto da tools/navi/maschera_manuale.py: il Ka-25PL del museo di Kiev e' azzurro-ciano, il cielo e' azzurro ma piu'
chiaro, l'erba e il cemento non sono blu.

Dentro `inviluppo` (poligono) e' velivolo il pixel con B - R > `soglia_br`, B - G < `max_bg` (il cielo e' molto piu'
blu che verde, la vernice ciano no) e luminanza < `soglia_lum`; poi chiusura
morfologica (`chiusura`), buchi fino a `buco_massimo` pixel riempiti (finestrini, ombre), macchie sotto `area_minima`
tolte; `includi` ed `escludi` (poligoni) come in maschera_manuale.py. Un file YAML puo' contenere piu' specifiche
separate da '---'. Produce la maschera (bianco = velivolo) e <uscita>_controllo.png.

Uso: elicotteri_maschere.py <radice progetto> <specifiche.yaml>
"""
import sys
import numpy as np, yaml
from PIL import Image, ImageDraw, ImageFilter
from scipy import ndimage

def poligono(shape, pts):
    im = Image.new('L', (shape[1], shape[0]), 0)
    if pts: ImageDraw.Draw(im).polygon([tuple(p) for p in pts], fill=255)
    return np.asarray(im) > 0

def costruisci(c, root):
    im = Image.open(f"{root}/{c['file']}").convert('RGB')
    rgb = np.asarray(im.filter(ImageFilter.GaussianBlur(c.get('sfocatura', 1.2))), float)
    lum = rgb @ np.array([0.299, 0.587, 0.114])
    m = (rgb[..., 2] - rgb[..., 0] > c.get('soglia_br', 12)) & (rgb[..., 2] - rgb[..., 1] < c.get('max_bg', 30)) & (lum < c.get('soglia_lum', 256))
    if c.get('inviluppo'): m &= poligono(m.shape, c['inviluppo'])
    k = c.get('chiusura', 5)
    if k: m = ndimage.binary_closing(m, np.ones((k, k)))
    for p in c.get('includi', []): m |= poligono(m.shape, p)
    for p in c.get('escludi', []): m &= ~poligono(m.shape, p)
    lab, n = ndimage.label(m)
    if n:
        dim = ndimage.sum(m, lab, range(1, n + 1)); m = np.isin(lab, 1 + np.flatnonzero(dim >= c.get('area_minima', 200)))
    buchi = ndimage.binary_fill_holes(m) & ~m
    lab, n = ndimage.label(buchi)
    if n:
        dim = ndimage.sum(buchi, lab, range(1, n + 1)); m |= np.isin(lab, 1 + np.flatnonzero(dim <= c.get('buco_massimo', 20000)))
    return m, np.asarray(im.convert('L'))

if __name__ == '__main__':
    root, spec = sys.argv[1], sys.argv[2]
    for c in yaml.safe_load_all(open(spec)):
        m, g = costruisci(c, root)
        Image.fromarray((m * 255).astype(np.uint8)).save(f"{root}/{c['uscita']}")
        rgb = np.stack([g] * 3, -1).astype(np.uint8); rgb[m] = (0.45 * rgb[m] + 0.55 * np.array([230, 40, 40])).astype(np.uint8)
        Image.fromarray(rgb).save(f"{root}/{c['uscita'][:-4]}_controllo.png")
        print(c['uscita'], 'pixel velivolo', int(m.sum()))
