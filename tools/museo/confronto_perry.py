"""Fogli di confronto del prototipo Perry: sprite originali 1989, foto trattate, modello 3D;
striscia dell'aspetto continuo; prova di scena al periscopio a 640x480 con la palette originale."""
import sys, numpy as np
from PIL import Image, ImageDraw

root = sys.argv[1]; d = f'{root}/extracted/prototipo_perry'
pal = list(open(f'{root}/extracted/raw/DISK1/DAY.PAL', 'rb').read())
BG = (88, 104, 124, 255)

def cella(im, w, h):
    im = im.convert('RGBA'); s = min(w / im.width, h / im.height)
    im = im.resize((max(1, round(im.width * s)), max(1, round(im.height * s))), Image.NEAREST)
    c = Image.new('RGBA', (w, h), BG); c.alpha_composite(im, ((w - im.width) // 2, h - im.height)); return c

viste = ['prua', 'prua 3/4', 'fianco', 'poppa 3/4', 'poppa']
orig = [Image.open(f'{root}/extracted/png/OHP{i}.png') for i in range(5)]
foto = [Image.open(f'{d}/museo/{n}_epoca16.png') for n in ('prua', 'prua_tre_quarti', 'fianco_sinistro', 'poppa_tre_quarti', 'poppa')]
# viste da dritta (prua a destra), come gli sprite originali 1-3
r3d = [Image.open(f'{d}/periscopio/{n}.png') for n in ('aob_000_dritta', 'aob_045_dritta', 'aob_090_dritta', 'aob_135_dritta', 'aob_180_dritta')]
r3f = [Image.open(f'{d}/periscopio_foto/{n}.png') for n in ('aob_000_dritta', 'aob_045_dritta', 'aob_090_dritta', 'aob_135_dritta', 'aob_180_dritta')]
W, H = 230, 150
righe = [('Sprite originali 1989', orig), ('Foto 1984-85 trattate (16 grigi)', foto), ('Modello 3D di prova (16 grigi)', r3d),
         ('Modello 3D misurato + foto proiettata (16 grigi)', r3f)]
foglio = Image.new('RGBA', (len(viste) * (W + 8) + 8, 30 + len(righe) * (H + 34)), (24, 24, 28, 255))
dr = ImageDraw.Draw(foglio)
for j, v in enumerate(viste): dr.text((8 + j * (W + 8) + W // 2 - 20, 8), v, fill=(230, 230, 230))
for i, (nome, ims) in enumerate(righe):
    y = 30 + i * (H + 34); dr.text((8, y), nome, fill=(240, 200, 120))
    for j, im in enumerate(ims): foglio.alpha_composite(cella(im, W, H), (8 + j * (W + 8), y + 16))
foglio.save(f'{d}/confronto.png')

# striscia dell'aspetto continuo (ogni 15 gradi, lato sinistro)
ims = [Image.open(f'{d}/periscopio_foto/aob_{a:03d}_sinistra.png') for a in range(0, 181, 15)]
s = 0.45; cw = round(530 * s); ch = round(135 * s)
st = Image.new('RGBA', (7 * (cw + 6) + 6, 2 * (ch + 22) + 6), (24, 24, 28, 255)); dr = ImageDraw.Draw(st)
for k, (a, im) in enumerate(zip(range(0, 181, 15), ims)):
    x = 6 + (k % 7) * (cw + 6); y = 6 + (k // 7) * (ch + 22)
    st.alpha_composite(cella(im, cw, ch), (x, y)); dr.text((x + 4, y + ch + 4), f'{a} gradi', fill=(220, 220, 220))
st.save(f'{d}/aspetto_continuo_foto.png')

# prova di scena al periscopio 640x480 (palette originale: cielo e mare dalle rampe blu, nave a 16 grigi)
sc = np.zeros((480, 640), np.uint8)
oriz = 250
for y in range(oriz): sc[y, :] = 63 - min(47, int(y / oriz * 47))                  # rampa cielo: piu' chiaro all'orizzonte
for y in range(oriz, 480): sc[y, :] = 110 - min(14, int((y - oriz) / 230 * 14))      # rampa mare scura 96..110
rng = np.random.default_rng(1)
onde = rng.random((480 - oriz, 640)) < 0.05; sc[oriz:][onde] = np.clip(sc[oriz:][onde] + 3, 96, 111)
nave = Image.open(f'{d}/periscopio_foto/aob_060_sinistra.png')
nave = nave.resize((round(nave.width * 0.55), round(nave.height * 0.55)), Image.NEAREST); k = np.array(nave)
sc_img = Image.fromarray(sc, 'P'); sc_img.putpalette(pal)
sc_img.paste(nave, (330 - k.shape[1] // 2, oriz - k.shape[0] + 6), Image.fromarray(((k != 159) * 255).astype(np.uint8)))
rgb = sc_img.convert('RGB'); dr = ImageDraw.Draw(rgb)
cx, cy, r = 320, 240, 232
mask = Image.new('L', (640, 480), 0); ImageDraw.Draw(mask).ellipse((cx - r, cy - r, cx + r, cy + r), fill=255)
fondo = Image.new('RGB', (640, 480), (0, 0, 0)); fondo.paste(rgb, mask=mask); rgb = fondo; dr = ImageDraw.Draw(rgb)
dr.line((cx - r, cy, cx + r, cy), fill=(10, 10, 10)); dr.line((cx, cy - r, cx, cy + r), fill=(10, 10, 10))
for t in range(-200, 201, 20): dr.line((cx + t, cy - 4, cx + t, cy + 4), fill=(10, 10, 10))
dr.text((24, 450), 'RLV 315   INGR. 6x   (prova di stile, valori indicativi)', fill=(200, 200, 200))
rgb.save(f'{d}/scena_periscopio_foto.png')
print('ok')
