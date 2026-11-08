"""Parti specifiche della corazzata classe Iowa (configurazione della riattivazione, anni '80).

Coordinate del kit (destrorse, metri): x verso prua, y verso sinistra, z verso l'alto.
Le parti sono costruite con la faccia (bocche da fuoco) verso +x e poi, se `verso` = -1, riflesse
rispetto a un piano trasversale: l'orientamento dei triangoli non conta per il rasterizzatore
(le normali vengono girate verso la fotocamera).

Parti:
  torre_trinata      torre da 16"/50 Mk 7: barbetta, casamatta con fronte inclinato, tre canne
  impianto_binato    impianto binato da 5"/38 Mk 28: casamatta con fronte smussato, due canne
  abl                lanciatore corazzato a cassone per Tomahawk (Mk 143 ABL), chiuso
  harpoon            quattro contenitori Harpoon (Mk 141) inclinati su un supporto
  fumaiolo_cappello  fumaiolo con cappello scuro inclinato
  direttore_mk38     direttore principale Mk 38 (casa con telemetro trasversale e cappuccio)
  direttore_mk37     direttore secondario Mk 37 (cilindro + casa con antenna)
  prisma_xz          profilo convesso nel piano x-z estruso tra y0 e y1
"""
import sys
sys.path.insert(0, '/home/randolph/Claude/melchior-688/tools/navi')
import numpy as np
import kit
from kit import _rifletti_x
from kit import Mesh, scatola, cilindro, tubo, asta, sfera


def harpoon(x, y, z0, lunghezza=4.6, inclinazione=12.0, verso=1.0):
    """Quattro contenitori Harpoon Mk 141 affiancati (2x2), inclinati verso l'esterno-prua, su supporto."""
    m = scatola(x - 1.2, x + 1.2, y - 0.9, y + 0.9, z0, z0 + 0.9, mat='traliccio')
    blocco = Mesh()
    for j in (-1, 1):
        for k in (-1, 1):
            blocco = blocco + tubo(-lunghezza / 2, lunghezza / 2, j * 0.42, k * 0.42, 0.38, n=6)
    blocco = blocco.ruota_y(-inclinazione).sposta(x, y, z0 + 1.6)
    m = m + blocco
    if verso < 0: m = _rifletti_x(m, x)
    return m


PARTI = dict(harpoon=harpoon)
