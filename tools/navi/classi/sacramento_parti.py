"""Parti proprie della classe Sacramento (AOE-1): stazioni di rifornimento in mare (kingpost con bracci
inclinati), fumaiolo cilindrico con cappello scuro, lanciatore Sea Sparrow Mk 29, gru, direttore di tiro
Mk 91, albero a palo con pennone. Coordinate del kit (x verso prua, y verso sinistra, z in alto, metri).

Le parti sottili (kingpost, bracci, alberi) usano i materiali 'traliccio' e 'scuro': nello sprite
coprono pochi pixel e, dove sono piu' sottili di un pixel, restano in parte trasparenti.
"""
import sys
sys.path.insert(0, '/home/randolph/Claude/melchior-688/tools/navi')
import numpy as np
import kit
from kit import Mesh, asta, scatola, cilindro, sfera

def lanciatore_mk29(x, z0, y=0.0, lunghezza=3.4, larghezza=2.6, altezza=1.5, verso=1.0):
    """Lanciatore ottuplo Mk 29 (NATO Sea Sparrow): piedistallo e cassone a celle."""
    m = cilindro(x, y, 0.8, z0, z0 + 0.9)
    return m + scatola(x - lunghezza / 2, x + lunghezza / 2, y - larghezza / 2, y + larghezza / 2,
                       z0 + 0.9, z0 + 0.9 + altezza, rastrema=0.96)

def albero_palo(x, z0, z_pennone, z_cima, semilunghezza_pennone=8.0, spessore=0.9, spessore_cima=0.35, y=0.0,
                pennone_spessore=0.35, mat='traliccio'):
    """Albero a palo: fusto fino al pennone, alberetto sottile fino alla cima, pennone trasversale."""
    m = asta((x, y, z0), (x, y, z_pennone), spessore, mat)
    m = m + asta((x, y, z_pennone), (x, y, z_cima), spessore_cima, mat)
    return m + asta((x, y - semilunghezza_pennone, z_pennone), (x, y + semilunghezza_pennone, z_pennone), pennone_spessore, mat)

PARTI = dict(lanciatore_mk29=lanciatore_mk29, albero_palo=albero_palo)
