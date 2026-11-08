"""Le bozze della carta F6 (proposte A e B del kit, scelte dell'utente del 29/10/2026): lancia engine/build_kit/bozze_carta,
che disegna dal codice vero la carta di oggi, la carta pulita (A) e la carta grande senza la striscia (B), e mette sotto
il pannello nuovo del gruppo 5 (docs/ricerca/25_bozze/stazioni/carta_A.png, approvato). Scrive in
docs/ricerca/25_bozze/carta/ le tre schermate e le tavole prima e dopo (oggi contro A, oggi contro B), a 2x.
Uso: bozze_carta.py [radice del progetto] [MISSIONE=SCENE1A]"""
import pathlib
import subprocess
import sys
import tempfile

from PIL import Image

RADICE = pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else '.').resolve()
MISSIONE = sys.argv[2] if len(sys.argv) > 2 else 'SCENE1A'
USCITA = RADICE / 'docs' / 'ricerca' / '25_bozze' / 'carta'
sys.path.insert(0, str(RADICE / 'tools' / 'estetica'))
from prima_dopo import tavola  # noqa: E402

TAVOLE = [('pulita', 'carta_pulita', 'LA CARTA PULITA (A): IL RETINO A 12 PX, UNA BARBA GRIGIA OGNI 192 PX, VENTO E MARE NELLA LEGENDA'),
          ('grande', 'carta_grande', 'LA CARTA GRANDE (B): COME A, SENZA LA STRISCIA IN ALTO; IL TITOLO RESTA NEL CARTIGLIO')]


def main():
    USCITA.mkdir(parents=True, exist_ok=True)
    pannello = Image.open(RADICE / 'docs' / 'ricerca' / '25_bozze' / 'stazioni' / 'carta_A.png').convert('RGB')
    pannello = pannello.resize((640, 480), Image.NEAREST).crop((0, 360, 640, 480))
    with tempfile.TemporaryDirectory() as d:
        subprocess.run([str(RADICE / 'engine' / 'build_kit' / 'bozze_carta'), str(RADICE / 'data' / 'motore'), d, MISSIONE],
                       check=True, capture_output=True, text=True,
                       env={'LD_LIBRARY_PATH': str(RADICE / 'tools/deps/sysroot/usr/lib/x86_64-linux-gnu')})
        schermate = {}
        for nome in ('carta_oggi', 'carta_pulita', 'carta_grande'):
            im = Image.new('RGB', (640, 480))
            im.paste(Image.open(pathlib.Path(d) / f'{nome}.bmp').convert('RGB'), (0, 0))
            im.paste(pannello, (0, 360))
            schermate[nome] = im
            dest = USCITA / f'{nome}.png'
            im.resize((1280, 960), Image.NEAREST).save(dest)
            print(dest.relative_to(RADICE))
    for sigla, nome, didascalia in TAVOLE:
        dest = USCITA / f'prima_dopo_carta_{sigla}.png'
        tavola(schermate['carta_oggi'], schermate[nome], didascalia).save(dest)
        print(dest.relative_to(RADICE))


if __name__ == '__main__':
    main()
