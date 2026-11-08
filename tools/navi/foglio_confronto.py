"""Fogli di confronto per una classe costruita con il kit: sprite originali 1989 (se presenti),
eventuali prove precedenti e sprite del kit nelle 5 viste dell'originale; striscia dell'aspetto
continuo; prova di scena al periscopio 640x480 con la palette originale.

Uso: foglio_confronto.py <radice> <cartella uscita classe> <prefisso sprite originali o '-'> [righe extra...]
Ogni riga extra e' 'titolo=cartella_sprite' (sprite aob_XXX_dritta.png).
Per gli oggetti in volo (sprite aob_XXX_lato_eNN.png) scrive anche elevazioni.png: una riga per elevazione e lato,
a scala fissa (2x), e nella scena di prova mette l'oggetto sopra l'orizzonte invece che sul mare.
"""
import glob, re
import sys, numpy as np
from PIL import Image, ImageDraw

root, out, orig = sys.argv[1], sys.argv[2], sys.argv[3]
extra = [a.split('=', 1) for a in sys.argv[4:]]
pal = list(open(f'{root}/extracted/raw/DISK1/DAY.PAL', 'rb').read())
BG = (88, 104, 124, 255)

def cella(im, w, h):
    im = im.convert('RGBA'); s = min(w / im.width, h / im.height)
    im = im.resize((max(1, round(im.width * s)), max(1, round(im.height * s))), Image.NEAREST)
    c = Image.new('RGBA', (w, h), BG); c.alpha_composite(im, ((w - im.width) // 2, h - im.height)); return c

viste = ['prua', 'prua 3/4', 'fianco', 'poppa 3/4', 'poppa']; aob = [0, 45, 90, 135, 180]
righe = []
if orig != '-': righe.append(('Sprite originali 1989', [Image.open(f'{root}/{orig}{i}.png') for i in range(5)]))
for titolo, cart in extra:
    righe.append((titolo, [Image.open(f'{root}/{cart}/aob_{a:03d}_dritta.png') for a in aob]))
righe.append(('Kit + proiezione multipla da foto calibrate', [Image.open(f'{root}/{out}/sprite/aob_{a:03d}_dritta.png') for a in aob]))
W, H = 230, 150
f = Image.new('RGBA', (len(viste) * (W + 8) + 8, 30 + len(righe) * (H + 34)), (24, 24, 28, 255)); dr = ImageDraw.Draw(f)
for j, v in enumerate(viste): dr.text((8 + j * (W + 8) + W // 2 - 20, 8), v, fill=(230, 230, 230))
for i, (nome, ims) in enumerate(righe):
    y = 30 + i * (H + 34); dr.text((8, y), nome, fill=(240, 200, 120))
    for j, im in enumerate(ims): f.alpha_composite(cella(im, W, H), (8 + j * (W + 8), y + 16))
f.save(f'{root}/{out}/confronto.png')

for lato in ('dritta', 'sinistra'):
    ims = [Image.open(f'{root}/{out}/sprite/aob_{a:03d}_{lato}.png') for a in range(0, 181, 15)]
    cw, ch = 240, 62
    st = Image.new('RGBA', (7 * (cw + 6) + 6, 2 * (ch + 22) + 6), (24, 24, 28, 255)); dr = ImageDraw.Draw(st)
    for k, (a, im) in enumerate(zip(range(0, 181, 15), ims)):
        x = 6 + (k % 7) * (cw + 6); y = 6 + (k // 7) * (ch + 22)
        st.alpha_composite(cella(im, cw, ch), (x, y)); dr.text((x + 4, y + ch + 4), f'{a} gradi ({lato})', fill=(220, 220, 220))
    st.save(f'{root}/{out}/aspetto_continuo_{lato}.png')

sc = np.zeros((480, 640), np.uint8); oriz = 250
for y in range(oriz): sc[y, :] = 63 - min(47, int(y / oriz * 47))
for y in range(oriz, 480): sc[y, :] = 110 - min(14, int((y - oriz) / 230 * 14))
rng = np.random.default_rng(1); onde = rng.random((480 - oriz, 640)) < 0.05
sc[oriz:][onde] = np.clip(sc[oriz:][onde] + 3, 96, 111)
nave = Image.open(f'{root}/{out}/sprite/aob_060_dritta.png')
in_volo = bool(glob.glob(f'{root}/{out}/sprite/aob_*_e*.png'))
if not in_volo: nave = nave.resize((round(nave.width * 0.55), round(nave.height * 0.55)), Image.NEAREST)
k = np.array(nave)
img = Image.fromarray(sc, 'P'); img.putpalette(pal)
img.paste(nave, (330 - k.shape[1] // 2, oriz - k.shape[0] + 6 - (90 if in_volo else 0)),
          Image.fromarray(((k != 159) * 255).astype(np.uint8)))
rgb = img.convert('RGB'); cx, cy, r = 320, 240, 232
mask = Image.new('L', (640, 480), 0); ImageDraw.Draw(mask).ellipse((cx - r, cy - r, cx + r, cy + r), fill=255)
fondo = Image.new('RGB', (640, 480), (0, 0, 0)); fondo.paste(rgb, mask=mask); dr = ImageDraw.Draw(fondo)
dr.line((cx - r, cy, cx + r, cy), fill=(10, 10, 10)); dr.line((cx, cy - r, cx, cy + r), fill=(10, 10, 10))
for t in range(-200, 201, 20): dr.line((cx + t, cy - 4, cx + t, cy + 4), fill=(10, 10, 10))
dr.text((24, 450), 'RLV 045   INGR. 6x   (prova di stile, valori indicativi)', fill=(200, 200, 200))
fondo.save(f'{root}/{out}/scena_periscopio.png')

# oggetti in volo: una riga per elevazione e lato a scala fissa
elev = sorted({int(m.group(1)) for f in glob.glob(f'{root}/{out}/sprite/aob_*_e*.png')
               for m in [re.search(r'_e(\d+)\.png$', f)] if m})
if elev:
    angoli = list(range(0, 181, 30)); righe_e = [(0, 'dritta'), (0, 'sinistra')] + [(e, l) for e in elev for l in ('dritta', 'sinistra')]
    def apri(a, l, e):
        return Image.open(f'{root}/{out}/sprite/aob_{a:03d}_{l}' + (f'_e{e:02d}' if e else '') + '.png').convert('RGBA')
    ims = [[apri(a, l, e) for a in angoli] for e, l in righe_e]
    cw = max(i.width for r in ims for i in r) * 2 + 8; ch = max(i.height for r in ims for i in r) * 2 + 8
    fe = Image.new('RGBA', (110 + len(angoli) * cw, 20 + len(righe_e) * ch), (24, 24, 28, 255)); dr = ImageDraw.Draw(fe)
    for j, a in enumerate(angoli): dr.text((110 + j * cw + cw // 2 - 10, 4), f'{a}', fill=(230, 230, 230))
    for i, ((e, l), riga) in enumerate(zip(righe_e, ims)):
        y = 20 + i * ch; dr.text((6, y + ch // 2 - 6), f'{l} elev {e}', fill=(240, 200, 120))
        for j, im in enumerate(riga):
            c = Image.new('RGBA', (cw, ch), BG)
            im2 = im.resize((im.width * 2, im.height * 2), Image.NEAREST)
            c.alpha_composite(im2, ((cw - im2.width) // 2, (ch - im2.height) // 2)); fe.alpha_composite(c, (110 + j * cw, y))
    fe.save(f'{root}/{out}/elevazioni.png')
print('ok')
