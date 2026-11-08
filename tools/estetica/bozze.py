"""Le tavole della rifinitura estetica (roadmap 33, dossier docs/ricerca/25_estetica_simulatori.md §8): lancia
engine/build_kit/bozze_estetica, incolla la carta del teatro (ritaglio dell'istantanea della scelta della missione di
trailer-1) nei rettangoli che il programma stampa, ingrandisce a 2x senza filtri e compone le tavole di confronto in
docs/ricerca/25_tavole/.
Uso: bozze.py [radice del progetto]"""
import pathlib
import subprocess
import sys
import tempfile

from PIL import Image, ImageDraw

RADICE = pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else '.').resolve()
USCITA = RADICE / 'docs' / 'ricerca' / '25_tavole'
PRIMA = RADICE / 'docs' / 'ricerca' / 'schermate_prima'
# la carta del teatro nell'istantanea di oggi (a 2x): x, y, larghezza, altezza
CARTA_PRIMA = (842, 222, 412, 224)


def main():
    USCITA.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory() as d:
        uscita = subprocess.run([str(RADICE / 'engine' / 'build_kit' / 'bozze_estetica'), str(RADICE / 'data' / 'motore'), d],
                                check=True, capture_output=True, text=True,
                                env={'LD_LIBRARY_PATH': str(RADICE / 'tools/deps/sysroot/usr/lib/x86_64-linux-gnu')}).stdout
        carte = {}
        for riga in uscita.splitlines():
            if riga.startswith('CARTA '):
                _, nome, x, y, w, h = riga.split()
                carte[nome] = tuple(map(int, (x, y, w, h)))
        prima = Image.open(PRIMA / 'menu_originali.png').convert('RGB')
        cx, cy, cw, ch = CARTA_PRIMA
        ritaglio = prima.crop((cx, cy, cx + cw, cy + ch)).resize((cw // 2, ch // 2), Image.NEAREST)
        tavole = {}
        for bmp in sorted(pathlib.Path(d).glob('*.bmp')):
            im = Image.open(bmp).convert('RGB')
            if bmp.stem in carte:
                x, y, w, h = carte[bmp.stem]
                im.paste(ritaglio.resize((w, h), Image.NEAREST), (x, y))
            tavole[bmp.stem] = im
            im.resize((1280, 960), Image.NEAREST).save(USCITA / f'{bmp.stem}.png')
    # i confronti: le tre varianti affiancate, e la combinazione consigliata (2x2), a scala 1
    def griglia(nomi, colonne, titolo, file):
        righe = (len(nomi) + colonne - 1) // colonne
        g = Image.new('RGB', (colonne * 640 + (colonne + 1) * 8, righe * 480 + (righe + 1) * 8 + 20), (20, 20, 24))
        ImageDraw.Draw(g).text((8, 4), titolo, fill=(220, 220, 220))
        for i, n in enumerate(nomi):
            im = tavole[n] if isinstance(n, str) and n in tavole else Image.open(n).convert('RGB').resize((640, 480), Image.NEAREST)
            g.paste(im, (8 + (i % colonne) * 648, 28 + (i // colonne) * 488))
        g.save(USCITA / file)
    griglia(['regolazioni_A', 'regolazioni_B', 'regolazioni_C'], 3, 'Regolazioni della sandbox: varianti A, B, C', 'confronto_regolazioni.png')
    griglia(['scelta_A', 'scelta_B', 'scelta_C'], 3, 'Scelta della missione: varianti A, B, C', 'confronto_scelta.png')
    griglia(['scelta_A', 'regolazioni_A', str(PRIMA / 'esito_rapporto.png'), 'statistiche_C'], 2,
            'Combinazione consigliata: A per menu e regolazioni, i documenti di B per l\'esito (come oggi), gli schermi di C per gli strumenti',
            'combinazione_consigliata.png')
    print('tavole in', USCITA)


if __name__ == '__main__':
    main()
