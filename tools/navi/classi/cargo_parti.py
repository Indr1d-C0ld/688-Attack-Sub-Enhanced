"""Parti proprie della nave portacontainer/ro-ro classe Cpl. Louis J. Hauge Jr. (ex Maersk, 1979).

Coordinate del kit (x verso prua, y verso sinistra, z verso l'alto, metri); i valori {prua: d} del
YAML arrivano gia' convertiti in x.
- torre_gru: colonna bianca della gru di bordo con testata trasversale, due corni dei bozzelli e
  braccio abbassato verso poppa (posizione di navigazione);
- pila_container: container impilati su un boccaporto, un blocco per strato (2,6 m) leggermente
  rientrato, cosi' le file restano visibili anche nell'ombreggiatura di ripiego;
- ponte_volo: piattaforma per elicotteri su pilastri, sopra la poppa.
"""
import sys
sys.path.insert(0, '/home/randolph/Claude/melchior-688/tools/navi')
import kit


def torre_gru(x, z0, z1, lato=3.2, traversa=7.0, testata=2.6, corna=1.6, braccio=36.0, z_braccio=30.0,
              z_punta=27.0, y_braccio=0.0, spessore_braccio=1.3, y=0.0):
    """Gru a colonna: fusto da z0 alla testata (sommita' z1 = cima dei corni), braccio verso poppa."""
    zt = z1 - corna                     # sommita' della testata
    h = lato / 2
    m = kit.scatola(x - h, x + h, y - h, y + h, z0, zt - testata, 'bianco', rastrema=0.92)
    m = m + kit.scatola(x - h * 1.1, x + h * 1.1, y - traversa / 2, y + traversa / 2, zt - testata, zt, 'bianco')
    for s in (-1, 1):                   # corni dei bozzelli ai due capi della testata
        yc = y + s * (traversa / 2 - 0.6)
        m = m + kit.scatola(x - 0.5, x + 0.5, yc - 0.45, yc + 0.45, zt, z1, 'bianco', rastrema=0.5)
    if braccio > 0:
        m = m + kit.asta((x - h, y + y_braccio, z_braccio), (x - braccio, y + y_braccio, z_punta), spessore_braccio, 'bianco')
    return m


def pila_container(x0, x1, y0, y1, z0, strati, altezza=2.6, rientro=0.12, mat='ponte'):
    m = kit.Mesh()
    for k in range(int(strati)):
        za, zb = z0 + k * altezza, z0 + (k + 1) * altezza - 0.05
        m = m + kit.scatola(x0 + rientro, x1 - rientro, y0 + rientro, y1 - rientro, za, zb, mat)
    return m


def ponte_volo(x0, x1, y0, y1, z, spessore=0.7, z_base=27.0, smusso=3.0, mat='scuro'):
    """Piattaforma ottagonale (spigoli smussati) con quattro pilastri verso poppa."""
    s = smusso
    prof = [(x0, y0 + s), (x0 + s, y0), (x1 - s, y0), (x1, y0 + s), (x1, y1 - s), (x1 - s, y1), (x0 + s, y1), (x0, y1 - s)]
    tri = []
    for zz, verso in ((z, -1), (z + spessore, 1)):
        P = [(px, py, zz) for px, py in prof]
        for k in range(1, len(P) - 1):
            tri.append((P[0], P[k], P[k + 1]) if verso > 0 else (P[0], P[k + 1], P[k]))
    for k in range(len(prof)):
        (ax, ay), (bx, by) = prof[k], prof[(k + 1) % len(prof)]
        tri += [((ax, ay, z), (bx, by, z), (bx, by, z + spessore)), ((ax, ay, z), (bx, by, z + spessore), (ax, ay, z + spessore))]
    m = kit.Mesh(tri, [mat] * len(tri))
    for px in (x0 + 2.0, (x0 + x1) / 2):
        for py in (y0 + 1.5, y1 - 1.5):
            m = m + kit.asta((px, py, z_base), (px, py, z), 0.45, 'bianco')
    return m


PARTI = dict(torre_gru=torre_gru, pila_container=pila_container, ponte_volo=ponte_volo)
