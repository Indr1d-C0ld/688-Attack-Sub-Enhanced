"""Le tavole del libretto di riconoscimento (docs/25_libretto_riconoscimento.md): lancia engine/build_kit/libretto_tavole,
salva le tavole grandi e mette ogni pagina del libretto sopra l'istantanea del periscopio di oggi
(docs/ricerca/schermate_prima/688_periscopio.png), nel terzo destro della vista. Tutto a 2x senza filtri, in
docs/ricerca/25_bozze/libretto/.
Uso: libretto_campione.py [radice del progetto] [classe...]"""
import pathlib
import subprocess
import sys
import tempfile

from PIL import Image

RADICE = pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else '.').resolve()
USCITA = RADICE / 'docs' / 'ricerca' / '25_bozze' / 'libretto'
TRASPARENTE = 159


def main():
    USCITA.mkdir(parents=True, exist_ok=True)
    vista = Image.open(RADICE / 'docs' / 'ricerca' / 'schermate_prima' / '688_periscopio.png').convert('RGB').resize((640, 480), Image.NEAREST)
    with tempfile.TemporaryDirectory() as d:
        out = subprocess.run([str(RADICE / 'engine' / 'build_kit' / 'libretto_tavole'), str(RADICE / 'data' / 'motore'), d] + sys.argv[2:],
                             check=True, capture_output=True, text=True,
                             env={'LD_LIBRARY_PATH': str(RADICE / 'tools/deps/sysroot/usr/lib/x86_64-linux-gnu')}).stdout
        print(out, end='')
        for bmp in sorted(pathlib.Path(d).glob('*.bmp')):
            im = Image.open(bmp)
            if bmp.stem.startswith('pagina_'):
                indici = im.convert('P') if im.mode != 'P' else im
                maschera = indici.point(lambda i: 0 if i == TRASPARENTE else 255, mode='L')
                composta = vista.copy()
                composta.paste(im.convert('RGB'), (0, 0), maschera)
                im = composta
            else:
                im = im.convert('RGB')
            dest = USCITA / f'{bmp.stem}.png'
            im.resize((im.width * 2, im.height * 2), Image.NEAREST).save(dest)
            print(dest.relative_to(RADICE))


if __name__ == '__main__':
    main()
