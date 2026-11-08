"""Le bozze della volta del ghiaccio (opzione A del sotto-ghiaccio, proposta del kit del 29/10/2026): lancia
engine/build_kit/bozze_strumenti (le tavole volta_fondale_C e volta_carta_A) e il gioco (engine/build_kit/melchior688,
SCENE1A: la pagina del fondale con Ctrl+H a 200 s, la carta della Navigazione a 600 s). Nella pagina mette sotto il
pannello di oggi; la striscia della carta (disegnata sull'indice 255, trasparente) la mette sopra la carta di oggi.
Scrive in docs/ricerca/25_bozze/stazioni/ le schermate e le tavole prima e dopo.
Uso: bozze_volta.py [radice del progetto]"""
import pathlib
import subprocess
import sys
import tempfile

from PIL import Image

RADICE = pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else '.').resolve()
USCITA = RADICE / 'docs' / 'ricerca' / '25_bozze' / 'stazioni'
sys.path.insert(0, str(RADICE / 'tools' / 'estetica'))
from prima_dopo import tavola  # noqa: E402


def gioco(d, nome, secondi, stazione, tasti=None):
    env = {'LD_LIBRARY_PATH': str(RADICE / 'tools/deps/sysroot/usr/lib/x86_64-linux-gnu'), 'XDG_DATA_HOME': str(d / 'xdg')}
    opzioni = []
    if tasti:
        (d / f'{nome}.txt').write_text(tasti)
        opzioni = ['--tasti-a-tempo', str(d / f'{nome}.txt')]
    subprocess.run([str(RADICE / 'engine' / 'build_kit' / 'melchior688'), '--missione', 'SCENE1A', '--regola', 'simulazione',
                    '--secondi', str(secondi), '--stazione', str(stazione), *opzioni, '--istantanea', str(d / f'{nome}.bmp')],
                   check=True, capture_output=True, text=True, env=env, cwd=RADICE / 'engine')
    return Image.open(d / f'{nome}.bmp').convert('RGB')


def main():
    USCITA.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory() as d:
        d = pathlib.Path(d)
        subprocess.run([str(RADICE / 'engine' / 'build_kit' / 'bozze_strumenti'), str(RADICE / 'data' / 'motore'), str(d)],
                       check=True, capture_output=True, text=True,
                       env={'LD_LIBRARY_PATH': str(RADICE / 'tools/deps/sysroot/usr/lib/x86_64-linux-gnu')})
        fondale = gioco(d, 'fondale', 200, 6, '100 quadro_fondale\n')
        carta = gioco(d, 'carta', 600, 6)
        # la pagina: sopra la bozza, sotto il pannello di oggi
        dopo = Image.open(d / 'volta_fondale_C.bmp').convert('RGB')
        dopo.paste(fondale.crop((0, 360, 640, 480)), (0, 360))
        # la carta: la striscia sopra la carta di oggi, dove l'indice non e' 255
        striscia = Image.open(d / 'volta_carta_A.bmp')
        maschera = striscia.point(lambda i: 0 if i == 255 else 255, mode='L')
        carta_dopo = carta.copy()
        carta_dopo.paste(striscia.convert('RGB'), (0, 0), maschera)
        for nome, prima, dopo_, didascalia in (
                ('volta_fondale', fondale, dopo,
                 'LA PAGINA DEL FONDALE CON LA VOLTA: LE CHIGLIE A PRUA DEL BQS-15, LA CARTA TERMICA SU E GIU\', LA SEZIONE COL GHIACCIO IN ALTO'),
                ('volta_carta', carta, carta_dopo,
                 'LA STRISCIA DELLA VOLTA SULLA CARTA: IL NASTRO DEL SONDATORE ALTO, LE POLYNYE CON L\'ETA\', LE CHIGLIE A PRUA')):
            dopo_.resize((1280, 960), Image.NEAREST).save(USCITA / f'{nome}.png')
            dest = USCITA / f'prima_dopo_{nome}.png'
            tavola(prima, dopo_, didascalia).save(dest)
            print(dest.relative_to(RADICE))


if __name__ == '__main__':
    main()
