"""Foglio di controllo dei modelli del kit: per ogni classe lo sprite di fianco e a 45 gradi (2x, stessa scala per
tutte), la sagoma nera e la vista al periscopio (Type 2 6x a 1,5 km, `anteprima_periscopio <dati> classe:<nome>`),
con gli errori di calibrazione di verifica.json.

Uso: foglio_controllo.py <radice> <anteprima_periscopio> <uscita.png> classe[:Nome] ...
Es.: foglio_controllo.py . engine/build_kit/anteprima_periscopio foglio.png delta3:"Delta III" ohio:Ohio
"""
import json, os, subprocess, sys, tempfile
import numpy as np
from PIL import Image, ImageDraw, ImageFont

SC = 2   # sprite a 2x


def carattere(grassetto, punti):
    nome = 'DejaVuSans-Bold.ttf' if grassetto else 'DejaVuSans.ttf'
    try:
        return ImageFont.truetype(f'/usr/share/fonts/truetype/dejavu/{nome}', punti)
    except OSError:
        return ImageFont.load_default()


def riga(radice, anteprima, cartella, c, nome):
    d = f'{radice}/extracted/navi/{c}/sprite'
    a = Image.open(f'{d}/aob_090_dritta.png').convert('RGBA')   # P con l'indice trasparente: l'alfa viene da li'
    b = Image.open(f'{d}/aob_045_dritta.png').convert('RGBA')
    pal = Image.open(f'{d}/aob_090_dritta.png')
    tr = pal.info.get('transparency', 0)   # l'indice trasparente degli sprite del kit (159), non lo 0 del nero
    sag = Image.new('RGB', pal.size, (240, 236, 224))
    sag.paste((20, 20, 20), mask=Image.fromarray(((np.asarray(pal) != tr) * 255).astype(np.uint8)))
    bmp = f'{cartella}/per_{c}.bmp'
    subprocess.run([anteprima, f'{radice}/data/motore', f'classe:{c}', bmp], check=True, stdout=subprocess.DEVNULL)
    per = Image.open(bmp).convert('RGB').crop((170, 25, 470, 335)).resize((450, 465), Image.NEAREST)
    fv = f'{radice}/extracted/navi/{c}/verifica.json'
    if os.path.exists(fv):
        ver = json.load(open(fv))['foto']
        note = '; '.join(f"{k}: {v['errore_punti_px']:.1f} px, sagoma {v['iou_finale']:.2f}" for k, v in ver.items())
    else:   # una classe riscalata dalla parente (tools/navi/riscala_classe.py): nessuna foto
        note = 'riscalata dalla parente, senza foto'
    return nome, c, a, b, sag, per, note


def main():
    radice, anteprima, uscita = sys.argv[1], os.path.abspath(sys.argv[2]), sys.argv[3]
    classi = [(x.split(':', 1) + [x])[:2] for x in sys.argv[4:]]
    F, f = carattere(True, 22), carattere(False, 15)
    with tempfile.TemporaryDirectory() as cartella:
        righe = [riga(radice, anteprima, cartella, c, nome) for c, nome in classi]
    W = 40 + max(max(r[2].size[0], r[3].size[0]) * SC for r in righe) + 40 + 450 + 40
    H = 60 + sum(60 + max(sum(x.size[1] * SC + 20 for x in r[2:5]), 465) + 40 for r in righe)
    out = Image.new('RGB', (W, H), (250, 248, 242)); dr = ImageDraw.Draw(out)
    dr.text((40, 18), 'Foglio di controllo dei modelli: sprite di fianco e a 45 gradi, sagoma, Type 2 6x a 1,5 km',
            fill=(0, 0, 0), font=F)
    y = 60
    for nome, c, a, b, sag, per, note in righe:
        dr.line([(30, y), (W - 30, y)], fill=(160, 150, 130), width=2)
        dr.text((40, y + 8), f'{nome}  ({c})', fill=(0, 0, 0), font=F)
        dr.text((40, y + 36), note, fill=(70, 70, 70), font=f)
        yy = y + 60
        for im in (a, b):
            big = im.resize((im.size[0] * SC, im.size[1] * SC), Image.NEAREST)
            fondo = Image.new('RGB', big.size, (60, 90, 120)); fondo.paste(big, mask=big.split()[3])
            out.paste(fondo, (40, yy)); yy += big.size[1] + 20
        out.paste(sag.resize((sag.size[0] * SC, sag.size[1] * SC), Image.NEAREST), (40, yy)); yy += sag.size[1] * SC + 20
        out.paste(per, (W - 40 - 450, y + 60))
        y = max(yy, y + 60 + 465) + 40
    out.save(uscita)
    print(uscita, out.size)


if __name__ == '__main__':
    main()
