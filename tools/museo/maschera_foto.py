"""Maschera nave/sfondo per una foto di fianco ripresa a livello del mare, piu' la sua calibrazione
(prua, poppa, galleggiamento -> metri). Sopra l'orizzonte la nave si separa dal cielo chiaro; tra
orizzonte e galleggiamento dal mare (soglia piu' bassa). Puntini isolati rimossi.
"""
import json, sys
import numpy as np
from PIL import Image, ImageFilter
from scipy import ndimage

def maschera(path, cal):
    g = np.asarray(Image.open(path).convert('L').filter(ImageFilter.GaussianBlur(0.8)), float)
    h, w = g.shape
    yy, xx = np.mgrid[0:h, 0:w]
    m = np.zeros((h, w), bool)
    sopra = yy < cal['orizzonte_y']
    tra = (yy >= cal['orizzonte_y']) & (yy <= cal['galleggiamento_y'])
    m |= sopra & (g < cal['soglia_cielo'])
    m |= tra & (g < cal['soglia_mare'])
    m &= (xx >= cal['prua_x'] - 2) & (xx <= cal['poppa_x'] + 2)
    m = ndimage.binary_opening(m, np.ones((2, 2)))
    lab, n = ndimage.label(m)
    if n:
        dim = ndimage.sum(m, lab, range(1, n + 1))
        tieni = 1 + np.flatnonzero(dim >= 30)
        m = np.isin(lab, tieni)
    # riempie i buchi solo sotto la coperta (numero di scafo dipinto in bianco), non nei tralicci
    pieno = ndimage.binary_fill_holes(m)
    m |= pieno & (yy > cal['coperta_min_y'])
    return m, g

# calibrazione della foto di fianco (USS Nicholas, 1984), coordinate nell'immagine a 960 px:
# prua (estremita' del dritto di prua) e poppa (specchio) a filo di coperta; galleggiamento.
CAL_NICHOLAS = dict(prua_x=70, poppa_x=860, galleggiamento_y=435, orizzonte_y=367,
                    soglia_cielo=185, soglia_mare=135, coperta_min_y=395, lunghezza_m=138.1, lato='sinistra')

if __name__ == '__main__':
    root = sys.argv[1]; d = f'{root}/extracted/prototipo_perry'
    m, g = maschera(f'{d}/foto/fianco_sinistro.jpg', CAL_NICHOLAS)
    cal = dict(CAL_NICHOLAS, m_per_px=CAL_NICHOLAS['lunghezza_m'] / (CAL_NICHOLAS['poppa_x'] - CAL_NICHOLAS['prua_x']))
    json.dump(cal, open(f'{d}/foto/fianco_sinistro_calibrazione.json', 'w'), indent=2)
    Image.fromarray((m * 255).astype(np.uint8)).save(f'{d}/foto/fianco_sinistro_maschera.png')
    ys, xs = np.nonzero(m)
    print('pixel nave', m.sum(), 'riquadro', xs.min(), xs.max(), ys.min(), ys.max(),
          'altezza massima sul galleggiamento', round((cal['galleggiamento_y'] - ys.min()) * cal['m_per_px'], 1), 'm')
