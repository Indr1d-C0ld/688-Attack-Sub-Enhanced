"""Contorno del modello proiettato su una foto calibrata, come poligono di partenza per una maschera a mano.

Usa la fotocamera salvata in extracted/<classe>/verifica.json (o in un file di fotocamere di triangola.py con
--fotocamere) e stampa il poligono in pixel della foto, pronto per `includi` o `inviluppo` di
maschera_manuale.py. Utile quando il mare chiaro fa perdere i fianchi dello scafo alla maschera automatica.

Uso: contorno_modello.py <classe.yaml> <nome della foto> [--parti scafo,chiusura_dritto] [--passo 12]
     [--fotocamere file.json] [--maschera uscita.png [--erodi k]]
Con --maschera scrive la sagoma proiettata come immagine (bianco = modello), adatta a sagome con buchi o a piu'
pezzi (vela e timoni, eliche): si usa come `maschera_file`/`maschera_calibrazione` o come base da ritoccare;
--erodi (o un valore negativo: dilata) la restringe di k pixel.
Senza --parti si usa tutto il modello. Il poligono segue, colonna per colonna, il bordo alto e il bordo basso
della sagoma (adatto a scafi e profili; non a sagome con buchi).
"""
import argparse, json, os, sys
import numpy as np, yaml
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from PIL import Image
import costruisci, raster
from camera import Camera, da_q

RADICE = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

def main():
    ap = argparse.ArgumentParser(); ap.add_argument('classe'); ap.add_argument('foto')
    ap.add_argument('--parti'); ap.add_argument('--passo', type=int, default=12); ap.add_argument('--fotocamere')
    ap.add_argument('--maschera'); ap.add_argument('--erodi', type=int, default=0)
    a = ap.parse_args()
    cl = yaml.safe_load(open(a.classe)); mesh = costruisci.costruisci_mesh(cl, os.path.dirname(os.path.abspath(a.classe)))
    fc = [f for f in cl['foto'] if f['nome'] == a.foto][0]
    w, h = Image.open(os.path.join(RADICE, fc['file'])).size
    h = round(h * float(fc.get('aspetto_pixel', 1.0)))
    if a.fotocamere:
        cam = da_q(json.load(open(a.fotocamere))[a.foto]['parametri'], w, h)
    else:
        ver = json.load(open(os.path.join(RADICE, cl['uscita'], 'verifica.json')))
        cam = Camera(ver['foto'][a.foto]['fotocamera'], w, h)
    tri = mesh.tri
    if a.parti:
        scelte = set(a.parti.split(','))
        tri = tri[np.array([p in scelte for p in mesh.parte])]
    sag = raster.sagoma(cam, tri, 1.0)
    if a.erodi:
        from scipy import ndimage
        sag = (ndimage.binary_erosion if a.erodi > 0 else ndimage.binary_dilation)(sag, iterations=abs(a.erodi))
    if a.maschera:
        Image.fromarray((sag * 255).astype(np.uint8)).save(a.maschera)
        print(f'maschera {a.maschera}: {int(sag.sum())} pixel'); return
    cols = np.flatnonzero(sag.any(axis=0))
    if not cols.size: sys.exit('il modello non cade nella foto')
    xs = list(range(cols[0], cols[-1] + 1, a.passo)) + [cols[-1]]
    alto, basso = [], []
    for x in xs:
        ys = np.flatnonzero(sag[:, x])
        if ys.size: alto.append([int(x), int(ys[0])]); basso.append([int(x), int(ys[-1])])
    poli = alto + basso[::-1]
    print(f"# contorno di {a.parti or 'tutto il modello'} sulla foto {a.foto} ({len(poli)} vertici)")
    print('- ' + json.dumps(poli))

if __name__ == '__main__':
    main()
