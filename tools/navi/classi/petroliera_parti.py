"""Parti proprie della petroliera (Sea Isle City, ex Umm al Maradem, 1981): passerella longitudinale sui
cavalletti e fascio delle tubolature di coperta, che seguono la coperta inclinata (nave in zavorra,
appoppata). Coordinate del kit (x verso prua, y verso sinistra, z in alto, metri).
"""
import sys
sys.path.insert(0, '/home/randolph/Claude/melchior-688/tools/navi')
import numpy as np
import kit
from kit import Mesh, asta, prisma_xz

def passerella(x0, z0, x1, z1, altezza=2.0, spessore=0.45, semilarghezza=0.7, y=0.0, cavalletti=12, mat='traliccio'):
    """Passerella (catwalk) da (x0, z0) a (x1, z1), quote della coperta sotto i due estremi: piano di
    calpestio `altezza` metri sopra la coperta, sostenuto da `cavalletti` montanti."""
    za, zb = z0 + altezza, z1 + altezza
    m = prisma_xz([(x0, za), (x1, zb), (x1, zb + spessore), (x0, za + spessore)], y - semilarghezza, y + semilarghezza, mat)
    for t in np.linspace(0, 1, cavalletti):
        x = x0 + (x1 - x0) * t; zc = z0 + (z1 - z0) * t
        m = m + asta((x, y, zc), (x, y, zc + altezza), 0.3, mat)
    return m

def tubolature(x0, z0, x1, z1, semilarghezza=9.0, altezza=1.1, y=0.0, mat='scuro'):
    """Fascio delle tubolature del carico sulla coperta: lastra bassa che segue la coperta da (x0, z0) a (x1, z1)."""
    return prisma_xz([(x0, z0), (x1, z1), (x1, z1 + altezza), (x0, z0 + altezza)], y - semilarghezza, y + semilarghezza, mat)

PARTI = dict(passerella=passerella, tubolature=tubolature)
