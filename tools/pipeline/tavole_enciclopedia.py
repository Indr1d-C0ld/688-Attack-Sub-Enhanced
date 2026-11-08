"""Tavole di controllo dell'enciclopedia: per ogni voce scritta di data/motore/enciclopedia.toml la pagina di
engine/build/anteprima_enciclopedia (immagini in gioco, scheda, testo) con accanto le foto digitalizzate «alla 1989»
(.img nei grigi della palette del giorno, a grandezza doppia), in extracted/enciclopedia/tavole/<id>.png.
Uso: tavole_enciclopedia.py <radice del progetto> [id ...]"""
import os, subprocess, sys, tomllib

import numpy as np
from PIL import Image


def foto(radice, img):
    d = open(os.path.join(radice, 'data', 'motore', img), 'rb').read()
    assert d[:4] == b'IMG1', img
    w, h = np.frombuffer(d[4:8], '<u2')
    pix = np.frombuffer(d[8:8 + int(w) * int(h)], np.uint8).reshape(int(h), int(w))
    pal = np.frombuffer(open(os.path.join(radice, 'data', 'motore', 'palette', 'giorno.pal'), 'rb').read(), np.uint8).reshape(256, 3)
    # 320x200 si vede a 4:3: raddoppio e stiramento a 640x480, poi meta' per la tavola (320x240)
    return Image.fromarray(pal[pix]).resize((320, 240), Image.NEAREST)


def tavole(radice, ids=None):
    enc = tomllib.load(open(os.path.join(radice, 'data', 'motore', 'enciclopedia.toml'), 'rb'))
    uscita = os.path.join(radice, 'extracted', 'enciclopedia', 'tavole')
    os.makedirs(uscita, exist_ok=True)
    fatte = []
    for vid, v in sorted(enc['voci'].items()):
        if ids and vid not in ids: continue
        bmp = os.path.join(uscita, vid + '.bmp')
        subprocess.run([os.path.join(radice, 'engine', 'build', 'anteprima_enciclopedia'), os.path.join(radice, 'data', 'motore'), vid, bmp],
                       check=True)
        pagina = Image.open(bmp).convert('RGB')
        fotos = [im for im in v.get('immagini', []) if im['tipo'] == 'foto'][:2]
        if fotos:
            t = Image.new('RGB', (640 + 320, 480))
            t.paste(pagina, (0, 0))
            for k, im in enumerate(fotos):
                t.paste(foto(radice, im['file']), (640, k * 240))
            pagina = t
        pagina.save(os.path.join(uscita, vid + '.png'))
        os.remove(bmp)
        fatte.append(vid)
    return fatte


if __name__ == '__main__':
    print(tavole(sys.argv[1] if len(sys.argv) > 1 else '.', sys.argv[2:] or None))
