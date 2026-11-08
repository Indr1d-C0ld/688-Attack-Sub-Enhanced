"""Prototipo: modello 3D semplice della fregata Oliver Hazard Perry (scafo lungo, FFG-36+) e resa
"da periscopio" a qualsiasi angolo sulla prua, quantizzata sui 16 grigi della palette originale.

Dimensioni dal dossier docs/ricerca/01_piattaforme_e_armi.md: lunghezza 138,1 m, larghezza 13,7 m.
Posizioni e altezze delle sovrastrutture sono stime ricavate dalle foto di fianco (USS Nicholas,
1984, pubblico dominio): da verificare con i piani d'epoca prima dell'uso definitivo, in particolare
l'altezza dell'albero (serve allo stadimetro).

Assi: x lungo lo scafo (prua = +), y verso dritta, z verso l'alto (linea di galleggiamento = 0). Metri.
"""
import os, sys, json
import numpy as np
from PIL import Image

L, B = 138.1, 13.7
XB, XS = L / 2, -L / 2          # prua, poppa
SLANCIO = 9.2                   # arretramento del dritto di prua al galleggiamento (misurato sulla foto)

# ---------------------------------------------------------------- geometria
def semilarghezza(x, z):
    """Semilarghezza dello scafo alla stazione x e quota z (svasatura verso l'alto, prua affilata)."""
    t = (XB - x) / 40.0                         # 0 alla prua, 1 a 40 m dalla prua
    prua = np.clip(t, 0, 1) ** 0.6
    poppa = 0.82 + 0.18 * np.clip((x - XS) / 30.0, 0, 1)   # specchio di poppa largo
    svasa = 1.0 + 0.04 * z / 7.0
    return B / 2 * prua * poppa * svasa

def bordo_libero(x):
    """Quota del ponte di coperta: cavallino verso prua."""
    return 5.8 + 3.1 * np.clip((x - 20) / (XB - 20), 0, 1) ** 1.5    # misurato sulla foto di fianco: 5,8 m al centro, 8,9 m al dritto

def scafo(n=60):
    xs = np.linspace(XS, XB, n)
    tri = []; col = []
    for i in range(n - 1):
        for x0, x1 in [(xs[i], xs[i + 1])]:
            z0a, z0b = bordo_libero(x0), bordo_libero(x1)
            # dritto di prua slanciato: oltre XB - SLANCIO il bordo inferiore della murata sale verso la coperta
            fondo = lambda x: np.clip((x - (XB - SLANCIO)) / SLANCIO, 0, 1) * bordo_libero(x) * 0.98
            for s in (1, -1):
                w = lambda x, z: s * semilarghezza(x, z)
                # fascia di carena: galleggiamento -> ponte, divisa in bagnasciuga nero (0-1 m) e grigio
                for za, zb, c in ((0.0, 1.0, 'nero'), (1.0, None, 'grigio')):
                    zt0 = zb if zb is not None else z0a; zt1 = zb if zb is not None else z0b
                    a0, a1 = max(za, fondo(x0)), max(za, fondo(x1))
                    zt0, zt1 = max(zt0, a0), max(zt1, a1)
                    p = [(x0, w(x0, a0), a0), (x1, w(x1, a1), a1), (x1, w(x1, zt1), zt1), (x0, w(x0, zt0), zt0)]
                    tri += [(p[0], p[1], p[2]), (p[0], p[2], p[3])]; col += [c, c]
            # ponte
            p = [(x0, -semilarghezza(x0, z0a), z0a), (x0, semilarghezza(x0, z0a), z0a),
                 (x1, semilarghezza(x1, z0b), z0b), (x1, -semilarghezza(x1, z0b), z0b)]
            tri += [(p[0], p[1], p[2]), (p[0], p[2], p[3])]; col += ['ponte', 'ponte']
    # specchio di poppa
    z0 = bordo_libero(XS); w0, w1 = semilarghezza(XS, 0), semilarghezza(XS, z0)
    p = [(XS, -w0, 0), (XS, w0, 0), (XS, w1, z0), (XS, -w1, z0)]
    tri += [(p[0], p[1], p[2]), (p[0], p[2], p[3])]; col += ['grigio', 'grigio']
    return tri, col

def scatola(x0, x1, hw, z0, z1, c='grigio', y0=0.0, rastrema=1.0):
    """Parallelepipedo (con eventuale rastremazione in alto) da x0 a x1, semilarghezza hw."""
    hb, ht = hw, hw * rastrema
    v = [(x0, y0 - hb, z0), (x1, y0 - hb, z0), (x1, y0 + hb, z0), (x0, y0 + hb, z0),
         (x0, y0 - ht, z1), (x1, y0 - ht, z1), (x1, y0 + ht, z1), (x0, y0 + ht, z1)]
    f = [(4, 5, 6, 7), (0, 1, 5, 4), (1, 2, 6, 5), (2, 3, 7, 6), (3, 0, 4, 7)]
    tri = []
    for a, b, cc, d in f:
        tri += [(v[a], v[b], v[cc]), (v[a], v[cc], v[d])]
    return tri, [c] * len(tri)

def cilindro(x, y, r, z0, z1, c='grigio', n=12):
    tri = []
    for k in range(n):
        a0, a1 = 2 * np.pi * k / n, 2 * np.pi * (k + 1) / n
        p0 = (x + r * np.cos(a0), y + r * np.sin(a0)); p1 = (x + r * np.cos(a1), y + r * np.sin(a1))
        tri += [((*p0, z0), (*p1, z0), (*p1, z1)), ((*p0, z0), (*p1, z1), (*p0, z1)),
                ((x, y, z1), (*p0, z1), (*p1, z1))]
    return tri, [c] * len(tri)

def sfera(x, y, z, r, c='bianco', n=10):
    tri = []
    for i in range(n):
        t0, t1 = np.pi * i / n, np.pi * (i + 1) / n
        for k in range(2 * n):
            a0, a1 = np.pi * k / n, np.pi * (k + 1) / n
            P = lambda t, a: (x + r * np.sin(t) * np.cos(a), y + r * np.sin(t) * np.sin(a), z + r * np.cos(t))
            tri += [(P(t0, a0), P(t1, a0), P(t1, a1)), (P(t0, a0), P(t1, a1), P(t0, a1))]
    return tri, [c] * len(tri)

def modello():
    parti = [scafo()]
    add = parti.append
    # (stime dalle foto) da prua verso poppa
    add(cilindro(47.5, 0, 1.4, 7.6, 9.0))                        # lanciatore Mk13, piedistallo
    add(scatola(46.0, 49.5, 0.35, 9.0, 9.9, rastrema=1.0))        # braccio del lanciatore
    add(scatola(21.0, 37.0, 5.6, 6.8, 13.8, rastrema=0.92))       # plancia e tuga prodiera
    add(scatola(30.5, 37.4, 5.45, 12.3, 13.1, 'nero', rastrema=0.95)) # fascia finestre della plancia
    add(scatola(25.0, 32.0, 2.2, 13.8, 15.6))                     # base del radome
    add(sfera(28.5, 0, 17.3, 1.9))                                # radome Mk92 CAS
    add(scatola(17.0, 20.0, 1.3, 13.8, 31.0, 'scuro', rastrema=0.35))  # albero a traliccio (sagoma piena, scura)
    add(scatola(18.2, 18.8, 4.2, 25.5, 26.1))                     # pennone
    add(scatola(18.0, 19.0, 0.4, 31.0, 34.0))                     # alberetto con antenne
    add(scatola(9.0, 10.2, 3.7, 19.0, 23.2, 'scuro'))             # antenna SPS-49 (graticcio)
    add(scatola(8.5, 10.5, 0.8, 12.5, 19.0))                      # supporto SPS-49
    add(scatola(-38.0, 21.0, 6.3, 5.8, 12.0, rastrema=0.97))      # tuga centrale
    add(scatola(-1.5, 1.5, 1.4, 12.0, 13.6))                      # cannone 76/62 Mk75 (torretta)
    add(scatola(1.5, 6.5, 0.15, 12.7, 13.0))                      # canna
    add(cilindro(-9.0, 0, 1.1, 12.0, 15.8))                       # STIR, supporto
    add(cilindro(-9.0, 0, 1.4, 15.8, 16.6, 'bianco'))             # STIR, antenna (semplificata)
    add(scatola(-17.0, -11.0, 2.3, 12.0, 15.5, rastrema=0.9))     # fumaiolo
    add(scatola(-50.0, -38.0, 6.3, 5.6, 12.6))                    # hangar gemelli
    add(cilindro(-44.0, 0, 0.9, 12.6, 14.6, 'bianco'))            # Phalanx sull'hangar
    tri, col = [], []
    for t, c in parti: tri += t; col += c
    return np.array(tri, float), col

ALBEDO = {'grigio': 0.66, 'ponte': 0.42, 'nero': 0.08, 'bianco': 0.95, 'scuro': 0.30, 'traliccio': 0.32}

# ---------------------------------------------------------------- resa
def rendi(tri, col, aob_deg, h=150, occhio=2.0, distanza=4000.0, ss=3, sole=(0.55, 0.35, 0.75)):
    """Vista da un periscopio con angolo sulla prua `aob_deg` (0 = prua verso l'osservatore,
    90 = fianco di dritta, 180 = poppa). Proiezione prospettica da lontano (quasi ortografica),
    parti sotto il galleggiamento escluse. `sole` e' espresso rispetto all'osservatore:
    (verso l'osservatore, verso la sua destra, verso l'alto). Ritorna immagine float (0..1, NaN = sfondo)."""
    a = np.radians(aob_deg)
    # osservatore lungo la direzione che forma l'angolo aob con la prua (sulla dritta per 0..180)
    ox, oy = np.cos(a) * distanza, np.sin(a) * distanza
    cam = np.array([ox, oy, occhio]); avanti = -cam / np.linalg.norm(cam)
    destra = np.cross(avanti, [0, 0, 1]); destra /= np.linalg.norm(destra)
    su = np.cross(destra, avanti)
    destra = -destra   # il sistema del modello (x prua, y dritta, z alto) e' sinistrorso: senza questo l'immagine e' speculare
    v = tri.reshape(-1, 3) - cam
    xc, yc, zc = v @ destra, v @ su, v @ avanti
    f = distanza                                                  # 1 unita' immagine = 1 m al bersaglio
    px, py = xc * f / zc, yc * f / zc
    scala = h * ss / 40.0                                         # 40 m di altezza nell'immagine
    W = int(160 / 40.0 * h * ss)
    X = px * scala + W / 2; Y = (h * ss) * 0.85 - py * scala
    X = X.reshape(-1, 3); Y = Y.reshape(-1, 3); Z = zc.reshape(-1, 3)
    n = np.cross(tri[:, 1] - tri[:, 0], tri[:, 2] - tri[:, 0]); n /= np.linalg.norm(n, axis=1, keepdims=True) + 1e-12
    # normali orientate verso l'osservatore (le primitive non hanno un verso coerente)
    cen = tri.mean(axis=1); verso = (cam - cen); n = np.where(((n * verso).sum(1) < 0)[:, None], -n, n)
    s = sole[0] * (-avanti) + sole[1] * destra + sole[2] * np.array([0, 0, 1.0]); s /= np.linalg.norm(s)
    # luce: ambiente 0.25 + diffusa 0.75; le superfici basse un po' piu' scure (ombre, sporco, riflessi del mare)
    zm = tri[:, :, 2].mean(axis=1)
    lum = np.array([ALBEDO[c] for c in col]) * (0.25 + 0.75 * np.clip(n @ s, 0, 1) + 0.15 * np.abs(n[:, 2])) \
          * (0.80 + 0.20 * np.clip(zm / 15.0, 0, 1))
    lum = np.clip(lum, 0, 1) ** 0.85
    H = h * ss
    zbuf = np.full((H, W), np.inf); img = np.full((H, W), np.nan)
    for k in range(len(tri)):
        xs, ys, zs = X[k], Y[k], Z[k]
        x0, x1 = int(max(np.floor(xs.min()), 0)), int(min(np.ceil(xs.max()), W - 1))
        y0, y1 = int(max(np.floor(ys.min()), 0)), int(min(np.ceil(ys.max()), H - 1))
        if x0 > x1 or y0 > y1: continue
        gx, gy = np.meshgrid(np.arange(x0, x1 + 1) + 0.5, np.arange(y0, y1 + 1) + 0.5)
        d = (ys[1] - ys[2]) * (xs[0] - xs[2]) + (xs[2] - xs[1]) * (ys[0] - ys[2])
        if abs(d) < 1e-9: continue
        l0 = ((ys[1] - ys[2]) * (gx - xs[2]) + (xs[2] - xs[1]) * (gy - ys[2])) / d
        l1 = ((ys[2] - ys[0]) * (gx - xs[2]) + (xs[0] - xs[2]) * (gy - ys[2])) / d
        l2 = 1 - l0 - l1
        m = (l0 >= 0) & (l1 >= 0) & (l2 >= 0)
        if not m.any(): continue
        z = l0 * zs[0] + l1 * zs[1] + l2 * zs[2]
        sub = zbuf[y0:y1 + 1, x0:x1 + 1]; upd = m & (z < sub)
        sub[upd] = z[upd]; img[y0:y1 + 1, x0:x1 + 1][upd] = lum[k]
    # sottocampionamento ss x ss (antialias), mantenendo lo sfondo dove prevale
    h2, w2 = H // ss, W // ss
    blk = img[:h2 * ss, :w2 * ss].reshape(h2, ss, w2, ss)
    cop = (~np.isnan(blk)).mean(axis=(1, 3))
    with np.errstate(all="ignore"):
        val = np.nanmean(blk, axis=(1, 3))
    return np.where(cop >= 0.5, val, np.nan), cop

def sprite16(img, ramp16):
    """Quantizza su 15 grigi (indici 144..158) con retinatura ordinata; 159 = trasparente."""
    from digitalizza import BAYER4
    h, w = img.shape
    g = np.nan_to_num(img, nan=0.0) * 255
    L = np.asarray(ramp16, float); step = (L[-1] - L[0]) / (len(L) - 1)
    t = BAYER4[np.arange(h)[:, None] % 4, np.arange(w)[None, :] % 4] * 0.7
    k = np.clip(np.round((g - L[0]) / step + t), 0, len(L) - 1).astype(np.uint8) + 144
    k[np.isnan(img)] = 159
    return k

def ritaglia(k):
    ys, xs = np.nonzero(k != 159)
    return k[ys.min():ys.max() + 1, xs.min():xs.max() + 1]

if __name__ == '__main__':
    root = sys.argv[1]
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    from digitalizza import rampa_originale
    out = f'{root}/extracted/prototipo_perry/periscopio'; os.makedirs(out, exist_ok=True)
    pal = list(open(f'{root}/extracted/raw/DISK1/DAY.PAL', 'rb').read())
    ramp = rampa_originale(root)
    tri, col = modello()
    print('triangoli', len(tri))
    for aob in range(0, 181, 15):
        img, cop = rendi(tri, col, aob)
        k = ritaglia(sprite16(img, ramp))
        im = Image.fromarray(k, 'P'); im.putpalette(pal); im.save(f'{out}/aob_{aob:03d}_dritta.png', transparency=159)
        im.transpose(Image.FLIP_LEFT_RIGHT).save(f'{out}/aob_{aob:03d}_sinistra.png', transparency=159)
        print(f'AOB {aob:3d}: {k.shape[1]}x{k.shape[0]} px')
