"""Tavole di verifica degli effetti visivi (docs/10_danni_affondamento_esplosioni.md §5, fase D4).

Lancia melchior688 in modalita' istantanea a piu' istanti per ogni scena, ritaglia la vista del periscopio e
compone una tavola PNG con le didascalie.

Uso: tavola_effetti.py USCITA.png SCENA [SCENA ...]
  SCENA = "titolo|argomenti di melchior688|t1,t2,...|ritaglio|zoom"
  (ritaglio facoltativo: x0,y0,x1,y1 della tela; zoom facoltativo: ingrandimento intero senza filtri)
Esempio:
  tavola_effetti.py tavola.png "Mk 48 sotto la chiglia|--colpo siluro_chiglia --rilevamento 160 --alto|0.5,3,12"
"""
import os, shlex, subprocess, sys, tempfile
from PIL import Image, ImageDraw, ImageFont

RADICE = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
ESEGUIBILE = os.path.join(RADICE, 'engine', 'build', 'melchior688')


def istante(argomenti, t, cartella):
    nome = os.path.join(cartella, f'{abs(hash((argomenti, t)))}.bmp')
    subprocess.run([ESEGUIBILE, *shlex.split(argomenti), '--secondi', str(t), '--istantanea', nome],
                   check=True, stdout=subprocess.DEVNULL)
    return Image.open(nome).convert('RGB')


def main():
    uscita, scene = sys.argv[1], sys.argv[2:]
    righe = []
    with tempfile.TemporaryDirectory() as cartella:
        for scena in scene:
            parti = scena.split('|')
            titolo, argomenti, tempi = parti[0], parti[1], [float(t) for t in parti[2].split(',')]
            ritaglio = tuple(int(v) for v in parti[3].split(',')) if len(parti) > 3 and parti[3] else (0, 30, 640, 360)
            zoom = int(parti[4]) if len(parti) > 4 else 1
            def taglia(im):
                im = im.crop(ritaglio)
                return im.resize((im.size[0] * zoom, im.size[1] * zoom), Image.NEAREST) if zoom > 1 else im
            righe.append((titolo, [(t, taglia(istante(argomenti, t, cartella))) for t in tempi]))
    w = max(im.size[0] for _, r in righe for _, im in r)
    h = max(im.size[1] for _, r in righe for _, im in r)
    colonne = max(len(r) for _, r in righe)
    TITOLO, MARGINE = 22, 4
    tavola = Image.new('RGB', (colonne * (w + MARGINE) + MARGINE, len(righe) * (h + TITOLO + MARGINE) + MARGINE), (24, 24, 24))
    disegno = ImageDraw.Draw(tavola)
    try:
        font = ImageFont.truetype('DejaVuSans.ttf', 14)
    except OSError:
        font = ImageFont.load_default()
    for i, (titolo, immagini) in enumerate(righe):
        y = MARGINE + i * (h + TITOLO + MARGINE)
        disegno.text((MARGINE + 2, y + 3), titolo, fill=(235, 235, 235), font=font)
        for j, (t, im) in enumerate(immagini):
            x = MARGINE + j * (w + MARGINE)
            tavola.paste(im, (x, y + TITOLO))
            m, sec = divmod(t, 60)
            etichetta = (f't = {t:g} s' if t < 120 else f't = {m:g} min' if sec == 0 else f't = {m:g} min {sec:g} s').replace('.', ',')
            disegno.rectangle((x, y + TITOLO, x + 8 + 8 * len(etichetta), y + TITOLO + 18), fill=(0, 0, 0))
            disegno.text((x + 4, y + TITOLO + 2), etichetta, fill=(255, 255, 120), font=font)
    tavola.save(uscita)
    print(uscita, tavola.size)


if __name__ == '__main__':
    main()
