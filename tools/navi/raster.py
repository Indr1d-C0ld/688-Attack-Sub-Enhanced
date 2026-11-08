"""Rasterizzazione software: sagome veloci (PIL) e G-buffer con z-buffer (numpy)."""
import numpy as np
from PIL import Image, ImageDraw

def sagoma(cam, tri, scala=1.0):
    """Maschera booleana della sagoma della mesh vista da `cam` (a risoluzione `scala`), sopra z = 0."""
    W, H = int(cam.w * scala), int(cam.h * scala)
    t = tri.copy(); t[..., 2] = np.maximum(t[..., 2], 0.0)
    u, v, z = cam.proietta(t.reshape(-1, 3))
    u = (u * scala).reshape(-1, 3); v = (v * scala).reshape(-1, 3); z = z.reshape(-1, 3)
    im = Image.new('L', (W, H), 0); dr = ImageDraw.Draw(im)
    ok = (z > 0.1).all(axis=1)
    for a, b in zip(u[ok], v[ok]):
        dr.polygon(list(zip(a, b)), fill=255)
    return np.asarray(im) > 0

def gbuffer(cam, tri, sopra_acqua=True):
    """Per ogni pixel: indice del triangolo visibile (-1 = sfondo), coordinate baricentriche, profondita'.
    I triangoli sono tagliati al galleggiamento portando a z = 0 i vertici sommersi (lo scafo sotto
    il mare non e' visibile)."""
    W, H = cam.w, cam.h
    t = tri.copy()
    if sopra_acqua: t[..., 2] = np.maximum(t[..., 2], 0.0)
    u, v, z = cam.proietta(t.reshape(-1, 3))
    U = u.reshape(-1, 3); V = v.reshape(-1, 3); Z = z.reshape(-1, 3)
    zbuf = np.full((H, W), np.inf); tid = np.full((H, W), -1, np.int32)
    l0b = np.zeros((H, W)); l1b = np.zeros((H, W))
    for k in range(len(t)):
        xs, ys, zs = U[k], V[k], Z[k]
        if (zs <= 0.1).any(): continue
        x0, x1 = int(max(np.floor(xs.min()), 0)), int(min(np.ceil(xs.max()), W - 1))
        y0, y1 = int(max(np.floor(ys.min()), 0)), int(min(np.ceil(ys.max()), H - 1))
        if x0 > x1 or y0 > y1: continue
        gx, gy = np.meshgrid(np.arange(x0, x1 + 1) + 0.5, np.arange(y0, y1 + 1) + 0.5)
        d = (ys[1] - ys[2]) * (xs[0] - xs[2]) + (xs[2] - xs[1]) * (ys[0] - ys[2])
        if abs(d) < 1e-12: continue
        l0 = ((ys[1] - ys[2]) * (gx - xs[2]) + (xs[2] - xs[1]) * (gy - ys[2])) / d
        l1 = ((ys[2] - ys[0]) * (gx - xs[2]) + (xs[0] - xs[2]) * (gy - ys[2])) / d
        l2 = 1 - l0 - l1
        m = (l0 >= 0) & (l1 >= 0) & (l2 >= 0)
        if not m.any(): continue
        # interpolazione prospetticamente corretta della profondita'
        zz = 1.0 / (l0 / zs[0] + l1 / zs[1] + l2 / zs[2])
        sub = zbuf[y0:y1 + 1, x0:x1 + 1]; upd = m & (zz < sub)
        if not upd.any(): continue
        sub[upd] = zz[upd]; tid[y0:y1 + 1, x0:x1 + 1][upd] = k
        # baricentriche corrette in prospettiva
        b0 = l0 / zs[0] * zz; b1 = l1 / zs[1] * zz
        l0b[y0:y1 + 1, x0:x1 + 1][upd] = b0[upd]; l1b[y0:y1 + 1, x0:x1 + 1][upd] = b1[upd]
    return tid, l0b, l1b, zbuf

def posizioni(tri, tid, l0, l1, sopra_acqua=True):
    """Coordinate del mondo dei pixel del G-buffer."""
    t = tri.copy()
    if sopra_acqua: t[..., 2] = np.maximum(t[..., 2], 0.0)
    k = np.maximum(tid, 0); T = t[k]
    l2 = 1 - l0 - l1
    return T[..., 0, :] * l0[..., None] + T[..., 1, :] * l1[..., None] + T[..., 2, :] * l2[..., None]
