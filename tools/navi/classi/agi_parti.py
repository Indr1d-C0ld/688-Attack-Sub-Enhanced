"""Parti proprie della classe AGI Okean (peschereccio convertito in nave raccolta informazioni), non presenti
nel kit condiviso.

- sfera_filo: antenna a gabbia sferica in cima agli alberi (le "sfere" della LINZA): tre anelli di filo
  (due meridiani ortogonali e l'equatore) fatti di aste sottili. Una sfera piena sembrerebbe una palla scura;
  le foto mostrano una gabbia trasparente di circa 1,3 m di diametro.
- anello: antenna ad anello (loop) su palo, un solo cerchio di aste nel piano verticale ruotato di
  `orientamento` gradi attorno a z (0 = piano x-z, cioe' parallelo all'asse della nave).
- verricello: verricello da pesca/salpancore: basamento, due tamburi orizzontali con l'asse trasversale (lungo y)
  e un carter centrale. Sulla LINZA ce n'e' uno grande davanti alla tuga di coperta (ex verricello da traino) e
  uno piu' piccolo a poppa.
- casotto_tondo: tuga con il cielo arrotondato (sezione trasversale a "D" capovolta): il casotto grigio sopra la
  plancia, da cui escono i fumi (vedi RAPPORTO.md). Corpo a pareti verticali e volta semicilindrica lungo x.
- gru_lancia: gruetta a gravita' della lancia (telaio a L rovesciata): montante e braccio fuoribordo.
- pannello_poppa: rivestimento chiaro della poppa tonda (nessuna foto vede la poppa: senza questo, con il
  materiale dello scafo, resterebbe un blocco scuro nelle viste da poppa). Riceve i parametri dello scafo.
"""
import sys
sys.path.insert(0, '/home/randolph/Claude/melchior-688/tools/navi')
import numpy as np
import kit
from kit import Mesh, scatola, cilindro, asta, _quad


def _cerchio(c, r, u, v, n, spessore, mat):
    """Cerchio di centro c e raggio r nel piano dei versori u, v, fatto di n aste."""
    c, u, v = (np.asarray(a, float) for a in (c, u, v))
    p = [c + r * (np.cos(2 * np.pi * k / n) * u + np.sin(2 * np.pi * k / n) * v) for k in range(n + 1)]
    m = Mesh()
    for a, b in zip(p[:-1], p[1:]): m = m + asta(a, b, spessore, mat)
    return m


def sfera_filo(x, z, r=0.65, y=0.0, n=12, spessore=0.07, mat='traliccio', asta_cima=1.0):
    """Gabbia sferica: meridiani nei piani x-z e y-z, equatore, piu' un'asta verticale passante e uno stilo in cima
    lungo `asta_cima` m."""
    c = (x, y, z)
    m = (_cerchio(c, r, (1, 0, 0), (0, 0, 1), n, spessore, mat) + _cerchio(c, r, (0, 1, 0), (0, 0, 1), n, spessore, mat)
         + _cerchio(c, r, (1, 0, 0), (0, 1, 0), n, spessore, mat))
    return m + asta((x, y, z - r), (x, y, z + r + asta_cima), spessore * 1.3, mat)


def anello(x, z, r=0.7, y=0.0, orientamento=0.0, n=12, spessore=0.07, mat='traliccio', palo=None, palo_base=None):
    """Antenna ad anello verticale; con `palo` (spessore) e `palo_base` (quota) aggiunge il palo di sostegno."""
    a = np.radians(orientamento)
    m = _cerchio((x, y, z), r, (np.cos(a), np.sin(a), 0.0), (0, 0, 1), n, spessore, mat)
    if palo:
        m = m + asta((x, y, palo_base), (x, y, z - r), palo, mat)
    return m


def _tamburo(x, y0, y1, z, r, mat, n=10):
    """Cilindro orizzontale con l'asse lungo y (tamburo di verricello)."""
    tri = []
    for k in range(n):
        a0, a1 = 2 * np.pi * k / n, 2 * np.pi * (k + 1) / n
        p = lambda y, a: (x + r * np.cos(a), y, z + r * np.sin(a))
        tri += _quad(p(y0, a0), p(y1, a0), p(y1, a1), p(y0, a1))
        tri += [((x, y0, z), p(y0, a1), p(y0, a0)), ((x, y1, z), p(y1, a0), p(y1, a1))]
    return Mesh(tri, [mat] * len(tri))


def verricello(x, z0, larghezza=4.0, r=0.55, lunghezza=1.8, y=0.0, mat='scuro'):
    """Basamento (lungo `lunghezza` in x, largo `larghezza` in y), due tamburi alle estremita' e carter centrale."""
    h = larghezza / 2
    m = scatola(x - lunghezza / 2, x + lunghezza / 2, y - h, y + h, z0, z0 + 0.35, 'sovrastruttura')
    zc = z0 + 0.35 + r
    for s in (-1, 1):
        m = m + _tamburo(x, y + s * h * 0.35, y + s * h, zc, r, mat)
    return m + scatola(x - lunghezza * 0.4, x + lunghezza * 0.4, y - h * 0.35, y + h * 0.35, z0 + 0.35, zc + r * 0.9,
                       'sovrastruttura')


def casotto_tondo(x0, x1, y0, y1, z0, z1, mat='sovrastruttura', n=8):
    """Tuga con volta a botte lungo x: pareti verticali fino a z1 - semilarghezza, poi mezza sezione circolare
    (appiattita se la tuga e' bassa)."""
    yc, h = (y0 + y1) / 2, (y1 - y0) / 2
    zr = max(z0, z1 - h)                     # quota dell'imposta della volta
    k = (z1 - zr) / h                         # schiacciamento della volta
    prof = [(yc - h, z0)] + [(yc - h * np.cos(np.pi * i / n), zr + k * h * np.sin(np.pi * i / n)) for i in range(n + 1)] + [(yc + h, z0)]
    tri = []
    for (ya, za), (yb, zb) in zip(prof[:-1], prof[1:]):
        tri += _quad((x0, ya, za), (x1, ya, za), (x1, yb, zb), (x0, yb, zb))
    for xx in (x0, x1):                        # testate (ventaglio dal centro della base)
        for (ya, za), (yb, zb) in zip(prof[:-1], prof[1:]):
            tri.append(((xx, yc, z0), (xx, ya, za), (xx, yb, zb)))
    return Mesh(tri, [mat] * len(tri))


def gru_lancia(x, y, z0, altezza=3.2, sbraccio=1.0, spessore=0.25, mat='sovrastruttura'):
    """Gruetta a L rovesciata: montante da z0 a z0 + altezza sul bordo (y) e braccio verso l'esterno."""
    s = 1.0 if y > 0 else -1.0
    top = (x, y, z0 + altezza)
    return asta((x, y, z0), top, spessore, mat) + asta(top, (x, y + s * sbraccio, z0 + altezza - 0.4), spessore, mat)


def pannello_poppa(mat='sovrastruttura', scostamento=0.04, **sc):
    """Pannello chiaro sulla faccia di poppa dello scafo del kit (x = -L/2), dal galleggiamento alla coperta."""
    m = kit.scafo(**sc); XB = sc['L'] / 2; x = -XB - scostamento
    z1 = float(m.quota_coperta(-XB)); w0 = float(m.semilarghezza(-XB, 0.0)); w1 = float(m.semilarghezza(-XB, z1))
    tri = [((x, -w0, 0.0), (x, w0, 0.0), (x, w1, z1)), ((x, -w0, 0.0), (x, w1, z1), (x, -w1, z1))]
    return Mesh(tri, [mat] * 2)


PARTI = dict(sfera_filo=sfera_filo, anello=anello, verricello=verricello, casotto_tondo=casotto_tondo,
             gru_lancia=gru_lancia, pannello_poppa=pannello_poppa)
