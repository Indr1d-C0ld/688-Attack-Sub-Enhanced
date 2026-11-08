"""Maschera nave/sfondo ritoccata a mano per una foto, da poligoni tracciati sull'immagine.

Specifica YAML:
  file: foto
  uscita: png della maschera (bianco = nave)
  scafo: poligono dello scafo (linea di coperta, dritto, galleggiamento), coordinate in pixel
  soglia_scafo: dentro lo scafo e' nave cio' che e' piu' scuro (esclude schiuma e onda di prua)
  linea_coperta: spezzata da poppa a prua; sopra di essa si cercano le sovrastrutture
  inviluppo: poligono largo attorno a sovrastrutture e alberi
  soglia_sovrastrutture: sopra la coperta e' nave cio' che e' piu' scuro del mare alle spalle
  escludi: poligoni da togliere comunque; includi: poligoni da aggiungere comunque
Produce anche <uscita>_controllo.png (nave in rosso sulla foto).

Metodo alternativo `metodo: colore` (per navi chiare, piu' luminose di cielo e mare; dalla classe
Sacramento): dentro `inviluppo` e' nave cio' che e' poco blu, B - (R+G)/2 < `soglia_blu` (predefinita 20);
opzionali `orizzonte_y` + `soglia_cielo` (sopra l'orizzonte: nave piu' scura di un cielo non blu),
`zona_chiaro` + `soglia_chiaro` (esclude la schiuma), `sfocatura`.
Metodo `metodo: cielo` (per navi chiare contro un cielo grigio, anche piu' chiare del cielo; dalle classi
Kirov e Iowa): sopra l'orizzonte e dentro `inviluppo` (facoltativo: tutta l'immagine) e' nave cio' che si
scosta dal cielo della propria riga di piu' di `soglia` livelli, dopo una `sfocatura` (predefinita 1,6) che
attenua la grana. Il cielo di ogni riga e' la mediana dei pixel liberi (fuori dall'inviluppo e sopra
l'orizzonte) oppure delle colonne `fasce_cielo: [[x0, x1], ...]`, levigata su `filtro_cielo` righe (15).
Sotto l'orizzonte, facoltativi `soglia_mare_chiaro` (nave piu' chiara del mare) o `soglia_mare_scuro`.
`colore: true`: lo scarto dal cielo si misura come distanza nel colore RGB (oggetti colorati contro un cielo
dello stesso grigio, es. livree rosse e blu); `fondo: polinomio` (con `grado`, predefinito 3): il cielo e' un
polinomio in (x, y) adattato ai pixel liberi invece della mediana di riga (cieli vignettati o a gradiente).
`banda_orizzonte: k`: le righe entro k pixel dalla linea dell'orizzonte (sfocata, con foschia) non contano come
nave; le parti della nave che la attraversano si aggiungono con `includi`.
Lo scafo, quando ha lo stesso tono del mare, si aggiunge con `includi`.

Orizzonte (metodi colore e cielo): `orizzonte_y` (riga) oppure `orizzonte: [[x0, y0], [x1, y1]]` (retta,
anche inclinata), con `margine_orizzonte` pixel di sicurezza (2).
Pulizia comune: `includi`, `escludi`, `chiusura` (lato del quadrato di chiusura morfologica, 0 = nessuna),
`area_minima`, `buco_massimo`.
Un file YAML puo' contenere piu' specifiche separate da '---'.
"""
import os, sys, numpy as np, yaml
from PIL import Image, ImageDraw, ImageFilter
from scipy import ndimage

def poligono(shape, pts):
    im = Image.new('L', (shape[1], shape[0]), 0)
    if pts: ImageDraw.Draw(im).polygon([tuple(p) for p in pts], fill=255)
    return np.asarray(im) > 0

def _sopra_orizzonte(c, shape, margine_predefinito=0):
    """Pixel sopra l'orizzonte (orizzonte_y oppure retta `orizzonte`), meno `margine_orizzonte`; None se
    la specifica non ha orizzonte."""
    H, W = shape; mg = c.get('margine_orizzonte', margine_predefinito)
    if 'orizzonte' in c:
        (x0, y0), (x1, y1) = c['orizzonte']
        yo = y0 + (y1 - y0) * (np.arange(W) - x0) / (x1 - x0)
        return np.arange(H)[:, None] < yo[None, :] - mg
    if 'orizzonte_y' in c:
        return np.broadcast_to(np.arange(H)[:, None] < c['orizzonte_y'] - mg, shape)
    return None

def _inviluppo(c, shape):
    return poligono(shape, c['inviluppo']) if c.get('inviluppo') else np.ones(shape, bool)

def _fondo_polinomiale(canale, libero, grado):
    """Polinomio in (x, y) di grado dato adattato (minimi quadrati) ai pixel liberi di un canale."""
    H, W = canale.shape
    ys, xs = np.nonzero(libero)
    passo = max(1, len(xs) // 40000); xs, ys = xs[::passo], ys[::passo]
    termini = [(i, j) for i in range(grado + 1) for j in range(grado + 1 - i)]
    xn, yn = xs / W, ys / H
    A = np.stack([xn ** i * yn ** j for i, j in termini], 1)
    coef, *_ = np.linalg.lstsq(A, canale[ys, xs], rcond=None)
    Y, X = np.mgrid[0:H, 0:W]; X = X / W; Y = Y / H
    return sum(cf * X ** i * Y ** j for cf, (i, j) in zip(coef, termini))

def _per_cielo(c, root):
    im = Image.open(f"{root}/{c['file']}").convert('L')
    g = np.asarray(im.filter(ImageFilter.GaussianBlur(c.get('sfocatura', 1.6))), float); H, W = g.shape
    if c.get('colore'):
        rgb = np.asarray(Image.open(f"{root}/{c['file']}").convert('RGB').filter(
            ImageFilter.GaussianBlur(c.get('sfocatura', 1.6))), float)
        canali = [rgb[..., k] for k in range(3)]
    else:
        canali = [g]
    sopra = _sopra_orizzonte(c, g.shape, 2)
    if sopra is None: sopra = np.ones(g.shape, bool)
    inv = _inviluppo(c, g.shape)
    if 'fasce_cielo' in c:
        libero = np.zeros(g.shape, bool)
        for x0, x1 in c['fasce_cielo']: libero[:, x0:x1] = True
    else:
        libero = ~inv & sopra
    def fondo(canale):
        if c.get('fondo') == 'polinomio':
            return _fondo_polinomiale(canale, libero, int(c.get('grado', 3)))
        cielo = np.full(H, np.nan)
        for y in range(H):
            v = canale[y, libero[y]]
            if v.size > 20: cielo[y] = np.median(v)
        ok = ~np.isnan(cielo)
        cielo = ndimage.uniform_filter1d(np.interp(np.arange(H), np.arange(H)[ok], cielo[ok]), c.get('filtro_cielo', 15))
        return np.broadcast_to(cielo[:, None], canale.shape)
    scarto = np.sqrt(sum((ch - fondo(ch)) ** 2 for ch in canali))
    m = inv & sopra & (scarto > c.get('soglia', 12))
    if 'soglia_mare_chiaro' in c: m |= inv & ~sopra & (g > c['soglia_mare_chiaro'])
    if 'soglia_mare_scuro' in c: m |= inv & ~sopra & (g < c['soglia_mare_scuro'])
    k = c.get('banda_orizzonte', 0)
    if k:
        banda = _sopra_orizzonte(dict(c, margine_orizzonte=-k), g.shape) & ~_sopra_orizzonte(dict(c, margine_orizzonte=k), g.shape)
        m &= ~banda
    return m, np.asarray(im)

def _per_colore(c, root):
    im = Image.open(f"{root}/{c['file']}").convert('RGB')
    rgb = np.asarray(im.filter(ImageFilter.GaussianBlur(c.get('sfocatura', 0.8))), float)
    blu = rgb[..., 2] - (rgb[..., 0] + rgb[..., 1]) / 2
    lum = rgb @ np.array([0.299, 0.587, 0.114])
    nave = blu < c.get('soglia_blu', 20)
    sopra = _sopra_orizzonte(c, blu.shape)
    if sopra is not None:            # sopra l'orizzonte: nave = piu' scuro del cielo (cielo non blu)
        nave = np.where(sopra, lum < c['soglia_cielo'], nave)
    m = _inviluppo(c, blu.shape) & nave
    if 'zona_chiaro' in c:
        z = poligono(blu.shape, c['zona_chiaro']); m &= ~(z & (lum > c['soglia_chiaro']))
    return m, np.asarray(im.convert('L'))

def costruisci(c, root='.'):
    if c.get('metodo') == 'colore':
        m, g = _per_colore(c, root)
        return _pulisci(m, c, area=60, buco=400), g
    if c.get('metodo') == 'cielo':
        m, g = _per_cielo(c, root)
        return _pulisci(m, c, area=80, buco=1500, chiusura=3), g
    im = Image.open(f"{root}/{c['file']}").convert('L')
    g = np.asarray(im.filter(ImageFilter.GaussianBlur(1.0)), float); H, W = g.shape
    scafo = poligono(g.shape, c['scafo']) & (g < c.get('soglia_scafo', 175))
    lc = np.array(c['linea_coperta'], float)
    xs = np.arange(W); yc = np.interp(xs, lc[:, 0], lc[:, 1], left=-1, right=-1)
    sopra = np.arange(H)[:, None] < yc[None, :]
    sov = poligono(g.shape, c['inviluppo']) & sopra & (g < c.get('soglia_sovrastrutture', 160))
    m = scafo | sov
    return _pulisci(m, c), np.asarray(im)

def _pulisci(m, c, area=40, buco=150, chiusura=0):
    """Correzioni a mano (includi/escludi) e pulizia: macchie isolate tolte, piccoli buchi chiusi."""
    for p in c.get('includi', []): m |= poligono(m.shape, p)
    for p in c.get('escludi', []): m &= ~poligono(m.shape, p)
    k = c.get('chiusura', chiusura)
    if k: m = ndimage.binary_closing(m, np.ones((k, k)))
    m = ndimage.binary_opening(m, np.ones((2, 2)))
    lab, n = ndimage.label(m)
    if n:
        dim = ndimage.sum(m, lab, range(1, n + 1)); m = np.isin(lab, 1 + np.flatnonzero(dim >= c.get('area_minima', area)))
    # piccoli buchi (portelli, ombre chiare) chiusi; i vuoti grandi dei tralicci restano
    buchi = ndimage.binary_fill_holes(m) & ~m
    lab, n = ndimage.label(buchi)
    if n:
        dim = ndimage.sum(buchi, lab, range(1, n + 1)); m |= np.isin(lab, 1 + np.flatnonzero(dim <= c.get('buco_massimo', buco)))
    return m

if __name__ == '__main__':
    root, spec = sys.argv[1], sys.argv[2]
    for c in yaml.safe_load_all(open(spec)):
        m, g = costruisci(c, root)
        Image.fromarray((m * 255).astype(np.uint8)).save(os.path.join(root, c['uscita']))
        rgb = np.stack([g] * 3, -1).astype(np.uint8); rgb[m] = (0.45 * rgb[m] + 0.55 * np.array([230, 40, 40])).astype(np.uint8)
        Image.fromarray(rgb).save(os.path.join(root, c['uscita'][:-4] + '_controllo.png'))
        print(c['uscita'], 'pixel nave', int(m.sum()))
