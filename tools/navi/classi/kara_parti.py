"""Parti specifiche dell'incrociatore antisommergibile classe Kara (Progetto 1134B "Berkut-B"), 1989.

Coordinate del kit (destrorse, metri): x verso prua, y verso SINISTRA, z verso l'alto.
Le parti ricevono coordinate x gia' risolte (i valori {prua: d} sono convertiti da costruisci.py).

Parti:
  tronco             tronco di piramide a base rettangolare con pianta in basso e in alto indipendenti
                     (torre piramidale, fumaiolo rastremato, tughe con fronte inclinato)
  shtorm             lanciatore binato B-187 del sistema Shtorm (SA-N-3 "Goblet"): piedistallo, giostra,
                     due bracci con i missili; `verso` +1 bracci verso prua, -1 verso poppa
  head_light         direttore di tiro "Head Light" (4R60 Grom) dello Shtorm: piedistallo, cabina,
                     due grandi riflettori laterali e due piccoli in alto; `orientamento` in gradi (0 = prua)
  top_sail           antenna MR-600 "Top Sail": graticcio largo inclinato all'indietro, sul suo supporto
  rastrub_inclinato  lanciatore quadruplo URPK-5 Rastrub (SS-N-14) del kit, inclinato verso l'alto a prua
  ak630              impianto AK-630 da 30 mm: basamento, torretta tonda bassa e canna
  ponte_elicotteri   lastra del ponte di volo poppiero con i fianchi chiusi fino allo scafo
"""
import sys
sys.path.insert(0, '/home/randolph/Claude/melchior-688/tools/navi')
import numpy as np
import kit
from kit import Mesh, scatola, cilindro, asta, tubo, paraboloide


def _quad(a, b, c, d):
    return [(a, b, c), (a, c, d)]


def shtorm(x, z0, y=0.0, verso=1.0, altezza_base=1.6, lunghezza_missile=6.2, elevazione=8.0):
    """Lanciatore binato B-187 (Shtorm, SA-N-3): piedistallo cilindrico, giostra a cassone, due bracci
    con un missile ciascuno (tubi da 0,6 m con ogive), leggermente elevati."""
    m = cilindro(x, y, 1.3, z0, z0 + altezza_base, n=14)
    zc = z0 + altezza_base
    m = m + scatola(x - 1.6, x + 1.6, y - 1.5, y + 1.5, zc, zc + 1.3, rastrema=0.85)
    bracci = Mesh()
    for s in (-1, 1):
        bracci = bracci + scatola(-1.0, 3.2, s * 1.05 - 0.25, s * 1.05 + 0.25, 1.0, 1.5, 'scuro')
        bracci = bracci + tubo(-lunghezza_missile * 0.35, lunghezza_missile * 0.65, s * 1.05, 0.8, 0.32)
    bracci = bracci.ruota_y(-elevazione)
    if verso < 0: bracci = bracci.ruota_z(180.0)
    return m + bracci.sposta(x, y, zc)


def head_light(x, z0, y=0.0, altezza_base=1.6, orientamento=0.0, disco=4.0, disco_piccolo=2.0, cabina=2.6):
    """Direttore Head Light: piedistallo, cabina, due grandi paraboloidi ai lati e due piccoli su un
    supporto sopra la cabina (alto circa 7 m dalla piattaforma, misurato sulla foto di fianco)."""
    m = cilindro(x, y, 1.1, z0, z0 + altezza_base, n=12)
    zc = z0 + altezza_base
    t = scatola(-1.4, 1.4, -1.5, 1.5, zc, zc + cabina, rastrema=0.92)
    t = t + cilindro(0.0, 0.0, 0.5, zc + cabina, zc + cabina + 0.9, n=8)
    for s in (-1, 1):
        t = t + paraboloide(1.0, s * (1.5 + disco / 2), zc + cabina * 0.65, disco, disco, profondita=0.8, n=8)
        t = t + asta((0.0, s * 1.5, zc + cabina * 0.65), (0.3, s * (1.5 + disco / 2), zc + cabina * 0.65), 0.35, 'scuro')
        t = t + paraboloide(0.6, s * 1.1, zc + cabina + 0.9 + disco_piccolo / 2, disco_piccolo, disco_piccolo,
                            profondita=0.4, n=6)
    t = t.ruota_z(orientamento)
    return m + t.sposta(x, y, 0.0)


def top_sail(x, z, larghezza=7.6, altezza=5.2, inclinazione=25.0, orientamento=0.0, y=0.0, sbarre=7,
             spessore=0.22, mat='scuro'):
    """Antenna MR-600 Top Sail: graticcio largo (cornice, sbarre orizzontali e montanti) centrato a quota z,
    inclinato all'indietro di `inclinazione` gradi, con la faccia verso prua prima della rotazione
    `orientamento` attorno a z. Il supporto e' un traliccio corto sotto l'antenna."""
    g = Mesh(); h = larghezza / 2
    for k in range(sbarre + 1):
        zz = -altezza / 2 + altezza * k / sbarre
        g = g + asta((0, -h, zz), (0, h, zz), spessore, mat)
    for yy in np.linspace(-h, h, 7):
        g = g + asta((0, yy, -altezza / 2), (0, yy, altezza / 2), spessore, mat)
    g = g.ruota_y(-inclinazione)
    # braccio del ricevitore davanti alla faccia
    g = g + asta((0.0, 0.0, -altezza / 2), (2.6, 0.0, -altezza * 0.15), 0.35, mat)
    g = g.ruota_z(orientamento)
    return g.sposta(x, y, z)


def rastrub_inclinato(x0, x1, z0, y=0.0, elevazione=12.0, larghezza=2.4, altezza=2.6, r_tubo=0.42, sporgenza=0.8):
    """Lanciatore quadruplo del kit (Rastrub, SS-N-14) ruotato verso l'alto a prua di `elevazione` gradi
    attorno al suo estremo poppiero in basso; sul Kara i due lanciatori stanno ai lati della plancia."""
    m = kit.lanciatore_quadruplo(x0, x1, z0, larghezza=larghezza, altezza=altezza, r_tubo=r_tubo, y=y,
                                 sporgenza=sporgenza)
    return m.ruota_y(elevazione, x0, z0)


def ponte_elicotteri(x0, x1, semilarghezza, z, spessore=0.6, z_fianchi=None, mat='ponte'):
    """Ponte di volo poppiero: lastra tra x0 e x1 (semilarghezza costante) a quota z; con `z_fianchi`
    aggiunge le fiancate chiuse sotto la lastra fino a quella quota (murata dello scafo)."""
    m = scatola(x0, x1, -semilarghezza, semilarghezza, z - spessore, z, mat)
    if z_fianchi is not None:
        for s in (-1, 1):
            m = m + scatola(x0, x1, s * semilarghezza - 0.15, s * semilarghezza + 0.15, z_fianchi, z - spessore, 'scafo')
    return m


PARTI = dict(shtorm=shtorm, head_light=head_light, top_sail=top_sail, rastrub_inclinato=rastrub_inclinato, ponte_elicotteri=ponte_elicotteri)
