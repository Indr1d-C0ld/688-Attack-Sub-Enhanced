"""Parti proprie della classe Spruance (DD-963), configurazione 1989 con ASROC.

Solo le parti che il kit non ha: il lanciatore ASROC Mk 16 (otto celle in due file da quattro, su
piedistallo brandeggiabile) e l'antenna dell'SPS-40 (riflettore a graticcio curvo, piu' largo che alto).
Le altre (Mk45, mack, Harpoon, Sea Sparrow, Phalanx, tralicci) vengono da kit.py.
"""
import sys
sys.path.insert(0, '/home/randolph/Claude/melchior-688/tools/navi')
import numpy as np
import kit


def asroc_mk16(x, z0, y=0.0, lunghezza=5.0, larghezza=3.4, altezza=2.3, elevazione=0.0, basamento=1.2,
               r_base=1.5, verso=1.0, base_larga=None):
    """Lanciatore ASROC Mk 16 / Mk 112: basamento cilindrico basso, affusto e cassone di otto celle
    (2 file da 4, a coppie sovrapposte) con la fronte divisa in quattro sportelli; a riposo e' orizzontale
    e rivolto verso prua (verso=+1). `elevazione` in gradi. `base_larga` = [lunghezza, larghezza] di una
    piattaforma bassa sotto il basamento (il ponte rialzato davanti alla plancia degli Spruance)."""
    m = kit.Mesh()
    if base_larga:
        bl, bw = base_larga
        m = m + kit.scatola(x - bl / 2, x + bl / 2, y - bw / 2, y + bw / 2, z0 - 0.01, z0 + 0.35)
        z0 = z0 + 0.35
    m = m + kit.cilindro(x, y, r_base, z0, z0 + basamento, n=14)
    # culla a forcella: due guance ai lati del cassone
    zc = z0 + basamento
    for s in (-1, 1):
        m = m + kit.scatola(x - 1.0, x + 1.0, y + s * (larghezza / 2 + 0.05), y + s * (larghezza / 2 + 0.45), zc,
                            zc + altezza * 0.75)
    # cassone: profilo laterale con la fronte leggermente rientrante in basso
    h = lunghezza / 2
    cassa = kit.scatola(-h, h, -larghezza / 2, larghezza / 2, 0.0, altezza, rastrema=0.97)
    # divisori fra le celle (sottili, scuri) sulla fronte
    for k in (-1, 0, 1):
        yy = k * larghezza / 4
        cassa = cassa + kit.scatola(h - 0.02, h + 0.05, yy - 0.06, yy + 0.06, 0.05, altezza - 0.05, 'scuro')
    cassa = cassa + kit.scatola(h - 0.02, h + 0.05, -larghezza / 2, larghezza / 2, altezza / 2 - 0.06,
                                altezza / 2 + 0.06, 'scuro')
    cassa = cassa.ruota_y(-elevazione, 0.0, 0.0)
    if verso < 0:
        cassa = cassa.ruota_z(180.0)
    return m + cassa.sposta(x, y, zc + 0.1)


def sps40(x, z0, larghezza=5.6, altezza=3.0, y=0.0, orientamento=0.0, curvatura=0.9, sbarre=5,
          spessore=0.22, mat='scuro', sostegno=1.2):
    """Antenna dell'SPS-40 (ricerca aerea a lunga portata): riflettore a graticcio a sezione di
    paraboloide, molto piu' largo che alto, con l'illuminatore su un braccio davanti al centro e un
    sostegno sotto. `orientamento` in gradi attorno a z (0 = faccia verso prua). `curvatura`: freccia
    del riflettore (m) ai bordi."""
    m = kit.Mesh(); h = larghezza / 2
    ys = np.linspace(-h, h, 9)
    xs = -curvatura * (ys / h) ** 2           # bordi arretrati: concavita' verso la faccia (+x)
    for k in range(sbarre + 1):
        zz = altezza * k / sbarre
        for a in range(len(ys) - 1):
            m = m + kit.asta((xs[a], ys[a], zz), (xs[a + 1], ys[a + 1], zz), spessore, mat)
    for a in (0, 2, 4, 6, 8):
        m = m + kit.asta((xs[a], ys[a], 0.0), (xs[a], ys[a], altezza), spessore, mat)
    # braccio dell'illuminatore e illuminatore
    m = m + kit.asta((0.0, 0.0, altezza * 0.5), (1.6, 0.0, altezza * 0.5), 0.18, mat)
    m = m + kit.scatola(1.5, 1.9, -0.3, 0.3, altezza * 0.5 - 0.3, altezza * 0.5 + 0.3, mat)
    # sostegno girevole
    m = m + kit.scatola(-0.5, 0.3, -0.5, 0.5, -sostegno, 0.0, 'traliccio')
    return m.ruota_z(orientamento).sposta(x, y, z0 + sostegno)


PARTI = dict(asroc_mk16=asroc_mk16, sps40=sps40)
