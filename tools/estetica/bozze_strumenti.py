"""Le bozze del gruppo 2 della rifinitura estetica (roadmap 33): lancia engine/build_kit/bozze_strumenti, incolla la
carta del replay (ritaglio dell'istantanea di oggi, docs/ricerca/schermate_prima/replay_quadro.png) nel rettangolo che
il programma stampa (e la carta del debriefing dalla sua istantanea), poi rimette sopra i riquadri «SOPRA»,
ingrandisce a 2x senza filtri e scrive in docs/ricerca/25_bozze/strumenti/ e, per briefing e debriefing,
docs/ricerca/25_bozze/documenti/.
Uso: bozze_strumenti.py [radice del progetto]"""
import pathlib
import subprocess
import sys
import tempfile

from PIL import Image

RADICE = pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else '.').resolve()
USCITA = RADICE / 'docs' / 'ricerca' / '25_bozze' / 'strumenti'
# la carta nelle istantanee di oggi (a 2x): istantanea, x0, y0, x1, y1
CARTE = {'replay': ('replay_quadro.png', (8, 36, 944, 756)), 'vivo': ('sandbox_vivo.png', (0, 32, 944, 756)), 'debriefing': ('debriefing_esito.png', (12, 136, 884, 844)),
         'museo': ('museo.png', (404, 108, 876, 364)), 'editor': ('editor_1.png', (0, 44, 898, 844)),
         'editor_unita': ('editor_2.png', (0, 44, 898, 844)), 'editor_briefing': ('editor_7.png', (0, 44, 898, 844)),
         'radio': ('688_radio.png', (0, 720, 1280, 960)), 'manovra': ('688_manovra.png', (0, 720, 1280, 960)),
         'armi': ('688_armi.png', (0, 720, 1280, 960)),
         'carta': ('688_carta.png', (0, 0, 1280, 720)),
         # la pagina del fondale con la volta sta nella Carta: sotto, i pannelli della Carta dalla sua istantanea
         'volta_fondale': ('688_carta.png', (0, 720, 1280, 960)),
         'radio_alfa': ('alfa_radio.png', (0, 720, 1280, 960)), 'manovra_alfa': ('alfa_manovra.png', (0, 720, 1280, 960)),
         'armi_alfa': ('alfa_armi.png', (0, 720, 1280, 960)), 'carta_alfa': ('alfa_carta.png', (0, 0, 1280, 720)),
         # il piede della cerimonia: l'istantanea fino a 428; il fascicolo dall'anteprima del kit (qui non ci sono carriere)
         'fascicolo_piede': ('../25_bozze/cerimonia/fascicolo_anteprima.png', (0, 0, 1280, 856)),
         'bacheca_piede': ('bacheca.png', (0, 0, 1280, 856)), 'esito_piede': ('esito_rapporto.png', (0, 0, 1280, 856))}
# le tavole del gruppo 3 vanno nella loro cartella; le altre in strumenti/
CARTELLE = {'briefing_A': 'documenti', 'debriefing_A': 'documenti', 'museo_A': 'documenti', 'albo_B': 'documenti',
            'radio_A': 'stazioni', 'manovra_C': 'stazioni', 'armi_C': 'stazioni', 'carta_A': 'stazioni',
            'fascicolo_piede_B': 'cerimonia', 'bacheca_piede_B': 'cerimonia', 'esito_piede_B': 'cerimonia',
            'volta_fondale_C': 'stazioni', 'radio_alfa_A': 'stazioni', 'manovra_alfa_C': 'stazioni', 'armi_alfa_C': 'stazioni', 'carta_alfa_A': 'stazioni'}


def main():
    USCITA.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory() as d:
        out = subprocess.run([str(RADICE / 'engine' / 'build_kit' / 'bozze_strumenti'), str(RADICE / 'data' / 'motore'), d],
                             check=True, capture_output=True, text=True,
                             env={'LD_LIBRARY_PATH': str(RADICE / 'tools/deps/sysroot/usr/lib/x86_64-linux-gnu')}).stdout
        carte, sopra = {}, {}
        for riga in out.splitlines():
            if riga.startswith('CARTA ') or riga.startswith('SOPRA '):
                tipo, nome, x, y, w, h = riga.split()
                r = tuple(map(int, (x, y, w, h)))
                if tipo == 'CARTA':
                    carte[nome] = r
                else:
                    sopra.setdefault(nome, []).append(r)
        for bmp in sorted(pathlib.Path(d).glob('*.bmp')):
            im = Image.open(bmp).convert('RGB')
            nome = bmp.stem.rsplit('_', 1)[0]
            if nome in carte:
                x, y, w, h = carte[nome]
                istantanea, ritaglio = CARTE[nome]
                carta = Image.open(RADICE / 'docs' / 'ricerca' / 'schermate_prima' / istantanea).convert('RGB').crop(ritaglio)
                # stesso rapporto dei lati: si taglia al centro, poi si riduce
                cw, ch = carta.size
                if cw / ch > w / h:
                    nw = round(ch * w / h)
                    c = carta.crop(((cw - nw) // 2, 0, (cw - nw) // 2 + nw, ch))
                else:
                    nh = round(cw * h / w)
                    c = carta.crop((0, (ch - nh) // 2, cw, (ch - nh) // 2 + nh))
                originale = im.copy()
                im.paste(c.resize((w, h), Image.LANCZOS), (x, y))
                for rx, ry, rw, rh in sopra.get(nome, []):   # i riquadri disegnati sopra la carta
                    im.paste(originale.crop((rx, ry, rx + rw, ry + rh)), (rx, ry))
            dest = USCITA.parent / CARTELLE.get(bmp.stem, 'strumenti') / f'{bmp.stem}.png'
            dest.parent.mkdir(parents=True, exist_ok=True)
            im.resize((im.width * 2, im.height * 2), Image.NEAREST).save(dest)
            print(dest.relative_to(RADICE))


if __name__ == '__main__':
    main()
