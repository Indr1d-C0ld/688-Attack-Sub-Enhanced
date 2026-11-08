"""Maschere oggetto/cielo per le foto di missili e bersagli in volo (classi tomahawk, harpoon, p700, bqm74).

Il metodo `cielo` di tools/navi/maschera_manuale.py lavora sulla luminosita': i missili dipinti (rosso, blu
scuro, arancio) hanno spesso la stessa luminosita' del cielo e si perdono. Qui si usa la distanza di colore
(RGB) dal cielo della propria riga, che separa bene anche le tinte con la stessa luminosita'.

Specifica YAML (piu' specifiche separate da '---'):
  file: foto;  uscita: png della maschera (bianco = oggetto)
  inviluppo: poligono largo attorno all'oggetto (fuori e' cielo, e da li' si stima il colore del cielo)
  soglia: distanza di colore minima dal cielo (predefinita 30), dopo una sfocatura `sfocatura` (1,2)
  filtro_cielo: righe su cui si leviga il colore del cielo (25)
  includi / escludi: poligoni aggiunti o tolti comunque; chiusura (5); area_minima (200); buco_massimo (5000)
  solo_maggiore: true per tenere solo la componente piu' grande (predefinito false)
  soglia_luminanza: per oggetti chiari davanti a uno sfondo non uniforme (es. un missile bianco sotto l'ala di
    un aereo grigio): dentro l'inviluppo e' oggetto cio' che e' piu' chiaro del valore dato, al posto della
    distanza di colore dal cielo
  soglia_rosso: per oggetti rossi o arancio (bersagli BQM-74) davanti a uno sfondo qualsiasi: e' oggetto cio' che ha
    R - max(G, B) maggiore del valore dato (esclude bianco, grigi, blu e giallo)
Produce anche <uscita>_controllo.png (oggetto in rosso).
Uso: missili_maschere.py <radice> <specifica.yaml>
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
    rgb = np.asarray(im.filter(ImageFilter.GaussianBlur(c.get('sfocatura', 1.2))), float); H, W, _ = rgb.shape
    inv = poligono((H, W), c['inviluppo']) if c.get('inviluppo') else np.ones((H, W), bool)
    libero = ~inv
    cielo = np.full((H, 3), np.nan)
    for y in range(H):
        v = rgb[y, libero[y]]
        if len(v) > 20: cielo[y] = np.median(v, axis=0)
    ok = ~np.isnan(cielo[:, 0]); yy = np.arange(H)
    for k in range(3):
        cielo[:, k] = ndimage.uniform_filter1d(np.interp(yy, yy[ok], cielo[ok, k]), c.get('filtro_cielo', 25))
    dist = np.sqrt(((rgb - cielo[:, None, :]) ** 2).sum(-1))
    m = inv & (dist > c.get('soglia', 30))
    if 'soglia_luminanza' in c:
        m = inv & (rgb @ np.array([0.299, 0.587, 0.114]) > c['soglia_luminanza'])
    if 'soglia_rosso' in c:
        m = inv & (rgb[..., 0] - np.maximum(rgb[..., 1], rgb[..., 2]) > c['soglia_rosso'])
    for p in c.get('includi', []): m |= poligono(m.shape, p)
    for p in c.get('escludi', []): m &= ~poligono(m.shape, p)
    k = c.get('chiusura', 5)
    if k: m = ndimage.binary_closing(m, np.ones((k, k)))
    m = ndimage.binary_opening(m, np.ones((2, 2)))
    lab, n = ndimage.label(m)
    if n:
        dim = ndimage.sum(m, lab, range(1, n + 1))
        tieni = [int(np.argmax(dim)) + 1] if c.get('solo_maggiore') else list(1 + np.flatnonzero(dim >= c.get('area_minima', 200)))
        m = np.isin(lab, tieni)
    buchi = ndimage.binary_fill_holes(m) & ~m
    lab, n = ndimage.label(buchi)
    if n:
        dim = ndimage.sum(buchi, lab, range(1, n + 1)); m |= np.isin(lab, 1 + np.flatnonzero(dim <= c.get('buco_massimo', 5000)))
    for p in c.get('escludi', []): m &= ~poligono(m.shape, p)
    return m, np.asarray(im.convert('L'))


if __name__ == '__main__':
    root, spec = sys.argv[1], sys.argv[2]
    for c in yaml.safe_load_all(open(spec)):
        if not c: continue
        m, g = costruisci(c, root)
        Image.fromarray((m * 255).astype(np.uint8)).save(f"{root}/{c['uscita']}")
        rgb = np.stack([g] * 3, -1).astype(np.uint8); rgb[m] = (0.45 * rgb[m] + 0.55 * np.array([230, 40, 40])).astype(np.uint8)
        Image.fromarray(rgb).save(f"{root}/{c['uscita'][:-4]}_controllo.png")
        print(c['uscita'], 'pixel oggetto', int(m.sum()))
