"""Parti proprie della classe "mercantile" (nave da carico generale tipo Leninskiy Komsomol, progetto 567,
modello della Ravenstvo). Coordinate del kit (x verso prua, y verso sinistra, z in alto, metri).

- (lo scafo con poppa a incrociatore, nato qui, e' ora `kit.scafo_composto`, usato come generatore dello scafo)
- gru_ke26: gru elettrica di bordo da 5 t (KE-26): colonna, cabina e braccio a traliccio in posizione di
  riposo (quasi orizzontale, verso prua o verso poppa).
- albero_carico: albero dei bighi pesanti (60 t): fusto, crocetta, alberetto e bigo pesante a riposo
  quasi verticale accanto al fusto.
- boccaporto: mastra e coperchio di una stiva (scatola bassa).

Le parti sottili (bracci, bighi, alberetti) usano i materiali 'traliccio' e 'scuro'.
"""
import sys
sys.path.insert(0, '/home/randolph/Claude/melchior-688/tools/navi')
import numpy as np
import kit
from kit import Mesh, asta, scatola, cilindro


def gru_ke26(x, y, z0, verso=1.0, colonna=3.0, r_colonna=0.8, cabina=(3.0, 2.4, 2.6), braccio=13.0,
             inclinazione=-13.0, spessore_braccio=0.55):
    """Gru KE-26: colonna cilindrica dal ponte z0, cabina in cima, braccio a riposo lungo x (verso +1 = verso
    prua) con una leggera pendenza (gradi, negativa = punta in basso)."""
    lx, ly, lz = cabina
    zc = z0 + colonna
    m = cilindro(x, y, r_colonna, z0, zc, 'sovrastruttura', n=10)
    m = m + scatola(x - lx / 2, x + lx / 2, y - ly / 2, y + ly / 2, zc, zc + lz, 'scuro', rastrema=0.9)
    a = np.radians(inclinazione)
    p0 = np.array([x + verso * lx * 0.4, y, zc + 0.3])
    p1 = p0 + braccio * np.array([verso * np.cos(a), 0.0, np.sin(a)])
    # braccio a traliccio: due correnti affiancati
    for dy in (-0.5, 0.5):
        m = m + asta(p0 + [0, dy, 0], p1 + [0, dy * 0.4, 0], spessore_braccio, 'traliccio')
    return m


def albero_carico(x, z0, z_crocetta, z_cima, r=0.75, semilunghezza_crocetta=2.5, z_alberetto=None,
                  bigo=None, spessore_bigo=0.8, y=0.0, mat='scuro'):
    """Albero dei bighi pesanti: fusto conico fino alla crocetta, alberetto sottile fino alla cima,
    crocetta trasversale; `bigo`: [[x, y, z], [x, y, z]] del bigo pesante a riposo (facoltativo)."""
    zt = z_crocetta if z_alberetto is None else z_alberetto
    m = cilindro(x, y, r, z0, zt, mat, n=10, r_alto=r * 0.7)
    m = m + asta((x, y, zt), (x, y, z_cima), r * 0.45, mat)
    m = m + asta((x, y - semilunghezza_crocetta, z_crocetta), (x, y + semilunghezza_crocetta, z_crocetta), 0.3, 'traliccio')
    if bigo is not None:
        m = m + asta(bigo[0], bigo[1], spessore_bigo, 'traliccio')
    return m


def boccaporto(x0, x1, semilarghezza, z0, altezza=1.6, mat='ponte'):
    """Boccaporto di stiva: mastra e coperchio a pannelli (scatola bassa, leggermente bombata)."""
    return scatola(x0, x1, -semilarghezza, semilarghezza, z0, z0 + altezza, mat, rastrema=0.97)


PARTI = dict(gru_ke26=gru_ke26, albero_carico=albero_carico, boccaporto=boccaporto)
