"""Le bozze della stazione Sonar (F8) in stile cascate SDR (proposta e del kit, 29/10/2026; docs/ricerca/30): lancia
engine/build_kit/bozze_sonar_sdr (la parte di sopra, 640x360, in quattro bozze per i due modi) e il gioco
(engine/build_kit/melchior688, la stazione di oggi a 200 s di SCENE1A, e di SCENE2B per l'Okean dell'Alfa; modo
moderno e 1989). Sotto ogni bozza mette il
pannello di oggi dello stesso modo; scrive in docs/ricerca/25_bozze/sonar_sdr/ le schermate e le tavole prima e dopo.
Uso: bozze_sonar_sdr.py [radice del progetto]"""
import pathlib
import subprocess
import sys
import tempfile

from PIL import Image

RADICE = pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else '.').resolve()
USCITA = RADICE / 'docs' / 'ricerca' / '25_bozze' / 'sonar_sdr'
sys.path.insert(0, str(RADICE / 'tools' / 'estetica'))
from prima_dopo import tavola  # noqa: E402

DIDASCALIE = {
    'A': 'A, IL QUADRO SDR: TRACKER COME SEGNALIBRI, FASCIO E CIECHI DISEGNATI, BAND PLAN, PICCHI, BARRA DELLO ZOOM, CONTATTI',
    'B': 'B, LA CONSOLLE BQQ-5: GLI STESSI PEZZI SU DUE CRT INCASSATI, I TASTI QUADRATI A LUCE ACCANTO',
    'C': 'C, L\'ANALIZZATORE: IL LOFAR DEL FASCIO A TUTTA LARGHEZZA, LA LIBRERIA SOVRAPPOSTA, IL CURSORE E IL DELTA, GLI SLOT',
    'D': 'D, L\'OKEAN DELL\'ALFA: ENISEJ A CASCATA POLARE, CIECO A TRATTEGGIO, SCALA L/P, FASCIO E TRACKER SUL BORDO',
}
# la stazione di oggi: SCENE1A per il 688 (A, B, C), SCENE2B per l'Alfa (D)
MISSIONE = {'A': 'SCENE1A', 'B': 'SCENE1A', 'C': 'SCENE1A', 'D': 'SCENE2B'}


def main():
    USCITA.mkdir(parents=True, exist_ok=True)
    env = {'LD_LIBRARY_PATH': str(RADICE / 'tools/deps/sysroot/usr/lib/x86_64-linux-gnu')}
    with tempfile.TemporaryDirectory() as d:
        d = pathlib.Path(d)
        subprocess.run([str(RADICE / 'engine' / 'build_kit' / 'bozze_sonar_sdr'), str(RADICE / 'data' / 'motore'), str(d)],
                       check=True, capture_output=True, text=True, env=env)
        for modo, opzioni in (('moderno', []), ('1989', ['--sonar', '1989'])):
            prime = {}
            for missione in sorted(set(MISSIONE.values())):
                subprocess.run([str(RADICE / 'engine' / 'build_kit' / 'melchior688'), '--missione', missione, '--regola', 'simulazione',
                                '--secondi', '200', '--stazione', '8', *opzioni, '--istantanea', str(d / f'prima_{missione}_{modo}.bmp')],
                               check=True, capture_output=True, text=True, env=env, cwd=RADICE / 'engine')
                prime[missione] = Image.open(d / f'prima_{missione}_{modo}.bmp').convert('RGB')
            prime['SCENE1A'].resize((1280, 960), Image.NEAREST).save(USCITA / f'prima_{modo}.png')
            prime['SCENE2B'].resize((1280, 960), Image.NEAREST).save(USCITA / f'prima_alfa_{modo}.png')
            for b in 'ABCD':
                prima = prime[MISSIONE[b]]
                dopo = Image.open(d / f'sonar_{b}_{modo}.bmp').convert('RGB')
                dopo.paste(prima.crop((0, 360, 640, 480)), (0, 360))   # il pannello di sotto resta quello di oggi
                dopo.resize((1280, 960), Image.NEAREST).save(USCITA / f'sonar_{b}_{modo}.png')
                dest = USCITA / f'prima_dopo_{b}_{modo}.png'
                tavola(prima, dopo, DIDASCALIE[b] + (' (1989)' if modo == '1989' else ' (MODERNO)')).save(dest)
                print(dest.relative_to(RADICE))


if __name__ == '__main__':
    main()
