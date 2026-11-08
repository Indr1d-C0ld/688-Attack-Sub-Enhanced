"""Parti proprie dell'incrociatore classe Slava (Progetto 1164 "Atlant").

Coordinate del kit: x verso prua, y verso sinistra, z verso l'alto (metri).
"""
import sys
sys.path.insert(0, '/home/randolph/Claude/melchior-688/tools/navi')
import numpy as np
import kit
from kit import Mesh


def bazalt(x_fronti, z_fronte, y_tubi, lunghezza=11.2, elevazione=16.0, r=1.15, z_ponte=7.5,
           sostegno=True, simmetrico=True, mat_sostegno='sovrastruttura'):
    """Batteria dei contenitori-lanciatori del P-500 Bazalt (SS-N-12): per ogni x in `x_fronti` (estremita'
    prodiera, la piu' alta) e ogni y in `y_tubi` un contenitore cilindrico inclinato di `elevazione` gradi
    verso prua, con l'anello della bocca piu' largo. Sotto ogni coppia, il sostegno inclinato (prisma dal ponte
    al fianco inferiore dei tubi). Con `simmetrico` la batteria e' ripetuta sull'altro lato (y -> -y)."""
    m = Mesh()
    a = np.radians(elevazione)
    lati = (1, -1) if simmetrico else (1,)
    for xf in x_fronti:
        xr = xf - lunghezza * np.cos(a); zr = z_fronte - lunghezza * np.sin(a)
        for s in lati:
            for yy in y_tubi:
                t = kit.tubo(-lunghezza, 0.0, 0.0, 0.0, r, n=12) + kit.tubo(-0.6, 0.15, 0.0, 0.0, r * 1.12, n=12)
                m = m + t.ruota_y(elevazione).sposta(xf, s * yy, z_fronte)
            if sostegno:
                ys = [s * y for y in y_tubi]
                y0, y1 = min(ys) - r * 0.9, max(ys) + r * 0.9
                prof = [(xr + 0.6, z_ponte), (xf - 2.0, z_ponte), (xf - 2.0, z_fronte - (2.0 * np.sin(a)) - r * 0.9),
                        (xr + 0.6, zr + 0.6 * np.sin(a) - r * 0.9)]
                m = m + kit.prisma_xz(prof, y0, y1, mat_sostegno)
    return m


def fumaioli_binati(x0, x1, z0, z1, y_int=0.6, y_est=5.0, rastrema=0.95, smusso_prua=0.0):
    """Due fumaioli affiancati (dritta e sinistra) a pianta rettangolare, come sullo Slava."""
    m = Mesh()
    for s in (1, -1):
        ya, yb = sorted((s * y_int, s * y_est))
        m = m + kit.scatola(x0, x1, ya, yb, z0, z1, rastrema=rastrema, smusso_prua=smusso_prua)
    return m


def top_dome(x, z0, r=2.45, altezza_torre=3.1, lato_torre=4.4, y=0.0):
    """Direttore Top Dome (radar 3R41 Volna dell'S-300F): torre a pianta quadrata e grande cupola."""
    h = lato_torre / 2
    m = kit.scatola(x - h, x + h, y - h, y + h, z0, z0 + altezza_torre, rastrema=0.95)
    m = m + kit.cilindro(x, y, r * 0.75, z0 + altezza_torre, z0 + altezza_torre + 0.5, n=14)
    return m + kit.sfera(x, y, z0 + altezza_torre + 0.5 + r * 0.55, r, 'sovrastruttura', n=12)


PARTI = dict(bazalt=bazalt, fumaioli_binati=fumaioli_binati, top_dome=top_dome)
