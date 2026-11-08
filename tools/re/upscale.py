"""Prove di up-scaling 320x200 -> 640x480 che preservano la palette (nessun colore nuovo).

Metodi:
  nitido   : 2x nearest -> 640x400, poi righe ripetute 5:6 -> 640x480 (aspetto CRT 4:3 corretto)
  scale2x  : Scale2x/EPX (solo colori originali) -> 640x400, poi righe 5:6 -> 640x480
  letterbox: 2x nearest -> 640x400 centrato con bande da 40 px (aspetto non corretto)
"""
import sys, numpy as np
from PIL import Image

def scale2x(a):
    p = np.pad(a, 1, mode='edge')
    B, D, E, F, H = p[:-2, 1:-1], p[1:-1, :-2], p[1:-1, 1:-1], p[1:-1, 2:], p[2:, 1:-1]
    c = (B != H) & (D != F)
    e0 = np.where(c & (D == B), D, E); e1 = np.where(c & (B == F), F, E)
    e2 = np.where(c & (D == H), D, E); e3 = np.where(c & (H == F), F, E)
    out = np.empty((a.shape[0] * 2, a.shape[1] * 2), a.dtype)
    out[0::2, 0::2], out[0::2, 1::2], out[1::2, 0::2], out[1::2, 1::2] = e0, e1, e2, e3
    return out

def stretch_rows(a, h):
    idx = (np.arange(h) * a.shape[0] // h)
    return a[idx]

def main(src, outdir, name):
    im = Image.open(src); pal = im.getpalette()
    a = np.array(im)
    if a.shape[1] < 320: a = np.pad(a, ((0, 0), (0, 320 - a.shape[1])), mode='edge')
    res = {
        'nitido': stretch_rows(a.repeat(2, 0).repeat(2, 1), 480),
        'scale2x': stretch_rows(scale2x(a), 480),
    }
    lb = np.zeros((480, 640), a.dtype); lb[40:440] = a.repeat(2, 0).repeat(2, 1); res['letterbox'] = lb
    for k, v in res.items():
        o = Image.fromarray(v, 'P'); o.putpalette(pal); o.save(f'{outdir}/{name}_{k}.png')

if __name__ == '__main__':
    main(*sys.argv[1:4])
