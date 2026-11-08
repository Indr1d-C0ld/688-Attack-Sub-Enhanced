"""Ritaglio di una foto con la griglia nelle coordinate dell'originale, per leggere i punti di calibrazione del kit.

Uso: griglia_foto.py <foto> <x0> <y0> <x1> <y1> <uscita.png> [passo=50] [larghezza=1600]
Le linee ogni `passo` pixel dell'originale, etichettate ogni due; il ritaglio si ingrandisce o si riduce a `larghezza`.
"""
import sys
from PIL import Image, ImageDraw, ImageFont


def main():
    foto, (x0, y0, x1, y1), uscita = sys.argv[1], map(int, sys.argv[2:6]), sys.argv[6]
    passo = int(sys.argv[7]) if len(sys.argv) > 7 else 50
    larghezza = int(sys.argv[8]) if len(sys.argv) > 8 else 1600
    k = larghezza / (x1 - x0)
    im = Image.open(foto).convert('RGB').crop((x0, y0, x1, y1)).resize((larghezza, round((y1 - y0) * k)), Image.LANCZOS)
    d = ImageDraw.Draw(im)
    try:
        f = ImageFont.truetype('/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf', 12)
    except OSError:
        f = ImageFont.load_default()
    for x in range((x0 // passo + 1) * passo, x1, passo):
        u = (x - x0) * k
        d.line([(u, 0), (u, im.height)], fill=(255, 255, 0) if x % (2 * passo) == 0 else (0, 200, 255), width=1)
        if x % (2 * passo) == 0: d.text((u + 2, 2), str(x), fill=(255, 255, 0), font=f)
    for y in range((y0 // passo + 1) * passo, y1, passo):
        v = (y - y0) * k
        d.line([(0, v), (im.width, v)], fill=(255, 255, 0) if y % (2 * passo) == 0 else (0, 200, 255), width=1)
        if y % (2 * passo) == 0: d.text((2, v + 2), str(y), fill=(255, 255, 0), font=f)
    im.save(uscita)


if __name__ == '__main__':
    main()
