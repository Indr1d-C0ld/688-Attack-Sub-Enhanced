"""Maschere aereo/cielo per foto con fondo a gradiente (vignettatura, cielo sfumato), per i velivoli da pattugliamento.

Il metodo `cielo` di tools/navi/maschera_manuale.py stima il cielo riga per riga dai pixel fuori dall'inviluppo: con
una forte vignettatura (fondo chiaro ai bordi, piu' scuro al centro) il cielo dentro l'inviluppo risulta diverso da
quello dei bordi e la maschera prende tutto. Qui il fondo e' un polinomio 2D (grado `grado`) adattato in modo robusto
(scarto dei pixel anomali, iterato) ai pixel fuori dall'inviluppo e a quelli dentro che non se ne scostano;
e' aereo cio' che si scosta dal fondo di piu' di `soglia` livelli (sfocatura preventiva `sfocatura`).

Specifica YAML (piu' specifiche separate da '---'):
  file, uscita, inviluppo, soglia, grado (4), sfocatura (1.5), includi, escludi, area_minima, buco_massimo, chiusura
  (pulizia come in maschera_manuale.py), e facoltativi elenchi di file di testo con poligoni
  (righe '- [[u, v], ...]' come quelle stampate da contorno_modello.py o aerei_eliche_maschera.py) da aggiungere a
  `includi` (chiave `includi_file`) o a `escludi` (chiave `escludi_file`).
Facoltativo `modello: {classe: <yaml>, foto: <nome>, erosione: px, parti: [...], escludi_parti: [...]}`: aggiunge la
  sagoma del modello proiettata con la fotocamera calibrata (verifica.json), erosa di `erosione` pixel, per le foto in
  cui le superfici in ombra hanno lo stesso tono delle nuvole (l'erosione tiene il bordo incerto fuori dalla maschera);
  con `dilatazione: px` i pixel trovati per soglia valgono solo entro quella distanza dalla sagoma (nuvole isolate).
`metodo: soglia_cielo` (invece del fondo polinomiale): e' aereo cio' che si scosta dal cielo della riga (come il metodo
  cielo di maschera_manuale.py, con `soglia`).

Uso: aerei_maschera.py <radice> <specifica.yaml>
"""
import sys, json, numpy as np, yaml
from PIL import Image, ImageFilter
sys.path.insert(0, '/home/randolph/Claude/melchior-688/tools/navi')
import maschera_manuale as mm


def fondo_polinomiale(g, libero, grado=4, iterazioni=4, k=2.5, passo=4):
    H, W = g.shape
    yy, xx = np.mgrid[0:H:passo, 0:W:passo]
    gs = g[::passo, ::passo]; ls = libero[::passo, ::passo]
    def base(x, y):
        x = x / W * 2 - 1; y = y / H * 2 - 1
        return np.stack([x ** i * y ** j for i in range(grado + 1) for j in range(grado + 1 - i)], -1)
    A = base(xx.astype(float), yy.astype(float)); ok = ls.copy()
    for _ in range(iterazioni):
        c, *_ = np.linalg.lstsq(A[ok], gs[ok], rcond=None)
        r = gs - A @ c; s = 1.4826 * np.median(np.abs(r[ok]))
        ok = ls & (np.abs(r) < k * max(s, 2.0))
    Y, X = np.mgrid[0:H, 0:W]
    return (base(X.astype(float), Y.astype(float)) @ c)


def _poligoni_da_file(f):
    out = []
    for riga in open(f):
        riga = riga.strip()
        if riga.startswith('- '): out.append(json.loads(riga[2:]))
    return out


def sagoma_modello(spec, root, shape):
    import os
    import costruisci, raster
    from camera import Camera
    from scipy import ndimage
    cl = yaml.safe_load(open(f"{root}/{spec['classe']}"))
    mesh = costruisci.costruisci_mesh(cl, os.path.dirname(os.path.abspath(f"{root}/{spec['classe']}")))
    sel = np.ones(len(mesh.tri), bool)
    if spec.get('parti'): sel &= np.array([any(p.startswith(q) for q in spec['parti']) for p in mesh.parte])
    if spec.get('escludi_parti'): sel &= ~np.array([any(p.startswith(q) for q in spec['escludi_parti']) for p in mesh.parte])
    ver = json.load(open(f"{root}/{cl['uscita']}/verifica.json"))
    cam = Camera(ver['foto'][spec['foto']]['fotocamera'], shape[1], shape[0])
    sag = raster.sagoma(cam, mesh.tri[sel], 1.0)
    k = int(spec.get('erosione', 0)); kd = int(spec.get('dilatazione', 0))
    return (ndimage.binary_erosion(sag, iterations=k) if k else sag), (ndimage.binary_dilation(sag, iterations=kd) if kd else None)


def costruisci(c, root):
    im = Image.open(f"{root}/{c['file']}").convert('L')
    g = np.asarray(im.filter(ImageFilter.GaussianBlur(c.get('sfocatura', 1.5))), float)
    inv = mm.poligono(g.shape, c['inviluppo'])
    if c.get('metodo') == 'soglia_cielo':
        cc = dict(c, metodo='cielo')
        m, _ = mm._per_cielo(dict(cc, file=c['file']), root)
    else:
        # dentro l'inviluppo: pixel liberi solo dove non sono troppo lontani dal fondo stimato sui bordi (iterazione)
        bg = fondo_polinomiale(g, ~inv, c.get('grado', 4))
        libero = ~inv | (np.abs(g - bg) < c['soglia'] * 0.5)
        bg = fondo_polinomiale(g, libero, c.get('grado', 4))
        m = inv & (np.abs(g - bg) > c['soglia'])
    if c.get('modello'):
        dentro, vicino = sagoma_modello(c['modello'], root, g.shape)
        if vicino is not None: m &= vicino          # soglia valida solo vicino alla sagoma (via le nuvole isolate)
        m |= inv & dentro
    c = dict(c)
    for f in c.get('includi_file', []): c['includi'] = list(c.get('includi', [])) + _poligoni_da_file(f"{root}/{f}")
    for f in c.get('escludi_file', []): c['escludi'] = list(c.get('escludi', [])) + _poligoni_da_file(f"{root}/{f}")
    return mm._pulisci(m, c, area=80, buco=1500, chiusura=3), np.asarray(im)


if __name__ == '__main__':
    root, spec = sys.argv[1], sys.argv[2]
    for c in yaml.safe_load_all(open(spec)):
        m, g = costruisci(c, root)
        Image.fromarray((m * 255).astype(np.uint8)).save(f"{root}/{c['uscita']}")
        rgb = np.stack([g] * 3, -1).astype(np.uint8); rgb[m] = (0.45 * rgb[m] + 0.55 * np.array([230, 40, 40])).astype(np.uint8)
        Image.fromarray(rgb).save(f"{root}/{c['uscita'][:-4]}_controllo.png")
        print(c['uscita'], 'pixel aereo', int(m.sum()))
