"""Prototipo 2: modello 3D della Perry con posizioni misurate sulla foto di fianco (USS Nicholas,
1984) e la foto proiettata sulle superfici, resa da periscopio a qualsiasi angolo sulla prua.

- Geometria: primitive di perry3d.py, posizioni e quote misurate sulla foto calibrata
  (foto/fianco_sinistro_calibrazione.json: 0,175 m/pixel, prua x=70, galleggiamento y=435).
- Texture: proiezione laterale (lungo l'asse trasversale) della foto in scala di grigi. Peso
  della foto = |n_y|^1.5 (superfici rivolte di fianco); il resto usa l'ombreggiatura del modello,
  con l'albedo tarato sul tono medio della nave nella foto.
- Tralicci (albero principale e supporto SPS-49): dove la foto mostra cielo il pixel e' trasparente.
- Controllo di coincidenza: sagoma del modello a 90 gradi contro maschera della foto (IoU).
"""
import json, os, sys
import numpy as np
from PIL import Image
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import perry3d as P
from digitalizza import rampa_originale, BAYER4
from maschera_foto import maschera, CAL_NICHOLAS

XB = P.XB
def X(da_prua):            # distanza dalla prua (m) -> coordinata x del modello
    return XB - da_prua

def modello_misurato():
    parti = [P.scafo()]; add = parti.append
    add(P.cilindro(X(30.0), 0, 1.3, 6.2, 10.0))                       # lanciatore Mk13 (piedistallo sulla coperta)
    add(P.scatola(X(31.5), X(28.0), 0.35, 10.0, 11.9))                # braccio
    add(P.scatola(X(66.0), X(40.2), 6.0, 5.8, 13.6, rastrema=0.93))   # tuga prodiera e plancia
    add(P.scatola(X(42.3), X(40.0), 5.85, 12.0, 12.8, 'nero', rastrema=0.95))  # finestre plancia
    add(P.scatola(X(48.5), X(45.0), 1.6, 13.6, 16.6))                 # base radome
    add(P.sfera(X(46.7), 0, 18.5, 1.5))                               # radome Mk92 CAS
    add(P.scatola(X(55.6), X(52.8), 1.4, 13.6, 21.0, 'traliccio', rastrema=0.6))   # traliccio SPS-49
    add(P.scatola(X(57.0), X(51.3), 0.5, 21.0, 25.2, 'traliccio'))    # antenna SPS-49 (graticcio), orientata come nella foto
    add(P.scatola(X(67.0), X(60.6), 2.8, 13.6, 29.7, 'traliccio', rastrema=0.15))  # albero principale
    add(P.scatola(X(64.6), X(63.0), 3.5, 25.2, 25.8, 'traliccio'))    # pennone
    add(P.scatola(X(64.2), X(63.4), 0.4, 29.7, 35.8, 'traliccio'))    # alberetto
    add(P.scatola(X(111.9), X(66.0), 6.3, 5.8, 12.2, rastrema=0.97))  # tuga centrale e hangar
    add(P.cilindro(X(73.4), 0, 1.0, 12.2, 14.4))                      # STIR, supporto
    add(P.cilindro(X(73.4), 0, 1.3, 14.4, 15.3, 'bianco'))            # STIR
    add(P.scatola(X(84.5), X(81.5), 1.4, 12.2, 13.4))                 # cannone 76/62 Mk75
    add(P.scatola(X(81.5), X(77.0), 0.15, 12.8, 13.1))                # canna
    add(P.scatola(X(97.0), X(91.0), 2.4, 12.2, 13.4, rastrema=0.95))  # fumaiolo
    add(P.cilindro(X(107.5), 0, 0.9, 12.2, 13.6, 'bianco'))           # Phalanx
    tri, col = [], []
    for t, c in parti: tri += t; col += c
    return np.array(tri, float), col

ALB = P.ALBEDO
# riquadri del numero di scafo '47' nella foto (x0, y0, x1, y1): a prua e a poppa
NUMERI_NICHOLAS = [(118, 398, 156, 420), (824, 404, 843, 416)]

def rendi_foto(tri, col, aob_deg, foto, msk, cal, h=150, occhio=2.0, distanza=4000.0, ss=3,
               sole=(0.55, 0.35, 0.75), tono=0.40, lato=1, foto_specchio=None):
    """lato = +1 osservatore sulla dritta, -1 sulla sinistra. La foto e' del lato sinistro: le facce di
    dritta usano `foto_specchio` (la stessa foto con le scritte cancellate, che altrimenti apparirebbero
    rovesciate)."""
    a = np.radians(aob_deg)
    cam = np.array([np.cos(a) * distanza, lato * np.sin(a) * distanza, occhio])
    if foto_specchio is None: foto_specchio = foto
    av = -cam / np.linalg.norm(cam); de = np.cross(av, [0, 0, 1.0]); de /= np.linalg.norm(de); su = np.cross(de, av)
    de = -de   # sistema del modello sinistrorso (vedi perry3d.rendi)
    v = tri.reshape(-1, 3) - cam
    xc, yc, zc = v @ de, v @ su, v @ av
    sc = h * ss / 40.0; W = int(160 / 40.0 * h * ss); H = h * ss
    Xp = (xc * distanza / zc * sc + W / 2).reshape(-1, 3); Yp = (H * 0.85 - yc * distanza / zc * sc).reshape(-1, 3)
    Zp = zc.reshape(-1, 3)
    n = np.cross(tri[:, 1] - tri[:, 0], tri[:, 2] - tri[:, 0]); n /= np.linalg.norm(n, axis=1, keepdims=True) + 1e-12
    cen = tri.mean(axis=1); n = np.where(((n * (cam - cen)).sum(1) < 0)[:, None], -n, n)
    s = sole[0] * (-av) + sole[1] * de + sole[2] * np.array([0, 0, 1.0]); s /= np.linalg.norm(s)
    zm = cen[:, 2]
    # albedo tarato: la tinta media della nave nella foto corrisponde al grigio del modello
    alb = np.array([ALB[c] for c in col]) / ALB['grigio'] * tono
    lum = alb * (0.35 + 0.65 * np.clip(n @ s, 0, 1) + 0.15 * np.abs(n[:, 2])) * (0.85 + 0.15 * np.clip(zm / 15, 0, 1))
    wside = np.abs(n[:, 1]) ** 1.5
    tral = np.array([c == 'traliccio' for c in col])
    zbuf = np.full((H, W), np.inf); out = np.full((H, W), np.nan)
    mpx = cal['m_per_px']; fh, fw = foto.shape
    for k in range(len(tri)):
        xs, ys, zs = Xp[k], Yp[k], Zp[k]
        x0, x1 = int(max(np.floor(xs.min()), 0)), int(min(np.ceil(xs.max()), W - 1))
        y0, y1 = int(max(np.floor(ys.min()), 0)), int(min(np.ceil(ys.max()), H - 1))
        if x0 > x1 or y0 > y1: continue
        gx, gy = np.meshgrid(np.arange(x0, x1 + 1) + 0.5, np.arange(y0, y1 + 1) + 0.5)
        dd = (ys[1] - ys[2]) * (xs[0] - xs[2]) + (xs[2] - xs[1]) * (ys[0] - ys[2])
        if abs(dd) < 1e-9: continue
        l0 = ((ys[1] - ys[2]) * (gx - xs[2]) + (xs[2] - xs[1]) * (gy - ys[2])) / dd
        l1 = ((ys[2] - ys[0]) * (gx - xs[2]) + (xs[0] - xs[2]) * (gy - ys[2])) / dd
        l2 = 1 - l0 - l1
        m = (l0 >= 0) & (l1 >= 0) & (l2 >= 0)
        if not m.any(): continue
        z = l0 * zs[0] + l1 * zs[1] + l2 * zs[2]
        sub = zbuf[y0:y1 + 1, x0:x1 + 1]; upd = m & (z < sub)
        if not upd.any(): continue
        # coordinate del modello del pixel -> pixel della foto (proiezione laterale)
        T = tri[k]
        wx = l0 * T[0, 0] + l1 * T[1, 0] + l2 * T[2, 0]; wz = l0 * T[0, 2] + l1 * T[1, 2] + l2 * T[2, 2]
        fx = np.clip((cal['prua_x'] + (XB - wx) / mpx).astype(int), 0, fw - 1)
        fy = np.clip((cal['galleggiamento_y'] - wz / mpx).astype(int), 0, fh - 1)
        src = foto if n[k, 1] <= 0 else foto_specchio
        fv = src[fy, fx] / 255.0; fm = msk[fy, fx]
        if tral[k]:
            visib = upd & fm                      # traliccio: dove la foto mostra cielo si vede attraverso
            val = np.where(fm, fv, np.nan)
        else:
            visib = upd
            w = np.where(fm, wside[k], 0.0)
            val = w * fv + (1 - w) * lum[k]
        sub[visib] = z[visib]; out[y0:y1 + 1, x0:x1 + 1][visib] = val[visib]
    h2, w2 = H // ss, W // ss
    blk = out[:h2 * ss, :w2 * ss].reshape(h2, ss, w2, ss)
    cop = (~np.isnan(blk)).mean(axis=(1, 3))
    with np.errstate(all='ignore'):
        val = np.nanmean(blk, axis=(1, 3))
    return np.where(cop >= 0.4, val, np.nan)

def sagoma_in_foto(tri, cal, shape):
    """Sagoma laterale del modello (proiezione ortogonale su x-z, sopra il galleggiamento) disegnata
    nelle coordinate della foto calibrata, per il confronto diretto con la maschera della foto."""
    from PIL import ImageDraw
    im = Image.new('L', (shape[1], shape[0]), 0); dr = ImageDraw.Draw(im)
    mpx = cal['m_per_px']
    for t in tri:
        pts = [(cal['prua_x'] + (XB - x) / mpx, cal['galleggiamento_y'] - max(z, 0.0) / mpx) for x, y, z in t]
        dr.polygon(pts, fill=255)
    return np.asarray(im) > 0

def coincidenza(sm, fm, cal):
    """IoU e scarti per colonne tra sagoma del modello e maschera della foto (vuoti dei tralicci riempiti)."""
    from scipy import ndimage
    fm = ndimage.binary_closing(fm, np.ones((5, 5)))
    fm = ndimage.binary_fill_holes(fm)
    zona = np.zeros_like(fm); zona[:cal['galleggiamento_y'] + 1, cal['prua_x'] - 3:cal['poppa_x'] + 4] = True
    a, b = sm & zona, fm & zona
    iou = (a & b).sum() / max((a | b).sum(), 1)
    return round(float(iou), 3), int((a & ~b).sum()), int((b & ~a).sum())

if __name__ == '__main__':
    root = sys.argv[1]; d = f'{root}/extracted/prototipo_perry'
    out = f'{d}/periscopio_foto'; os.makedirs(out, exist_ok=True)
    cal = json.load(open(f'{d}/foto/fianco_sinistro_calibrazione.json'))
    msk, _ = maschera(f'{d}/foto/fianco_sinistro.jpg', CAL_NICHOLAS)
    foto = np.asarray(Image.open(f'{d}/foto/fianco_sinistro.jpg').convert('L'), float)
    # livelli estesi sui soli pixel della nave (percentili 2-98), come per le immagini del Museo
    p2, p98 = np.percentile(foto[msk], [2, 98])
    foto = np.clip((foto - p2) / (p98 - p2) * 235 + 10, 0, 255)
    tono = float(foto[msk].mean()) / 255
    pal = list(open(f'{root}/extracted/raw/DISK1/DAY.PAL', 'rb').read()); ramp = rampa_originale(root)
    tri, col = modello_misurato()
    # controllo di coincidenza a 90 gradi (lato sinistro = immagine specchiata della vista da dritta)
    res = {}
    for nome, (t, c) in (('modello_prova', P.modello()), ('modello_misurato', (tri, col))):
        sm = sagoma_in_foto(t, cal, msk.shape)
        iou, solo_modello, solo_foto = coincidenza(sm, msk, cal)
        res[nome] = dict(iou=iou, pixel_solo_modello=solo_modello, pixel_solo_foto=solo_foto)
        ov = np.zeros(msk.shape + (3,), np.uint8)
        ov[..., 1] = msk * 200; ov[..., 0] = sm * 230; ov[..., 2] = sm * 60
        Image.fromarray(ov[180:445, 40:900]).save(f'{out}/coincidenza_{nome}.png')
    print('coincidenza della sagoma di fianco con la foto:', res)
    json.dump(dict(iou_sagoma_90=res, tono_medio_foto=round(tono, 3)), open(f'{out}/verifica.json', 'w'), indent=2)
    # copia della foto senza il numero di scafo (riquadri nelle coordinate della foto), per il lato dritto
    senza = foto.copy()
    for x0, y0, x1, y1 in NUMERI_NICHOLAS:
        banda = foto[y0:y1, max(x0 - 25, 0):x0]; senza[y0:y1, x0:x1] = np.median(banda)
    for aob in range(0, 181, 15):
        for nome, lato in (('dritta', 1), ('sinistra', -1)):
            img = rendi_foto(tri, col, aob, foto, msk, cal, tono=tono, lato=lato, foto_specchio=senza)
            k = P.ritaglia(P.sprite16(img, ramp))
            im = Image.fromarray(k, 'P'); im.putpalette(pal)
            im.save(f'{out}/aob_{aob:03d}_{nome}.png', transparency=159)
    print('viste salvate in', out)
