"""Parti specifiche dell'incrociatore classe Kirov (Progetto 1144 "Orlan"), configurazione del Kirov 1989.

Coordinate del kit (destrorse, metri): x verso prua, y verso sinistra, z verso l'alto.

Parti:
  tronco            tronco di piramide a base rettangolare con pianta di base e di sommita' indipendenti
                    (torre piramidale dell'albero prodiero, basamento dell'albero poppiero); a differenza di
                    `scatola` la rastremazione in lunghezza e in larghezza e' separata e la sommita' puo'
                    essere spostata verso prua o verso poppa
  poppa_slanciata   cuneo che completa lo specchio di poppa inclinato (lo scafo del kit ha lo specchio
                    verticale): profilo triangolare dal galleggiamento allo spigolo superiore arretrato
  top_pair          radar Top Pair (MR-800 Voskhod): due riflettori a graticcio schiena contro schiena
                    (Top Sail grande e cilindrico, Big Net ovale) su una colonna
  lanciatore_binato lanciatore a due bracci su piedistallo (URPK-3 Metel / SS-N-14 del Kirov)
"""
import sys
sys.path.insert(0, '/home/randolph/Claude/melchior-688/tools/navi')
import numpy as np
import kit
from kit import Mesh, _quad, _mesh, scatola, cilindro, asta, prisma_xz


def tronco(x0, x1, semilarghezza, z0, z1, x0_alto=None, x1_alto=None, semilarghezza_alta=None, y=0.0,
           mat='sovrastruttura'):
    """Tronco di piramide: base x0..x1 per +-semilarghezza a quota z0, sommita' x0_alto..x1_alto per
    +-semilarghezza_alta a quota z1 (valori mancanti = come la base)."""
    xa = x0 if x0_alto is None else x0_alto
    xb = x1 if x1_alto is None else x1_alto
    wa = semilarghezza if semilarghezza_alta is None else semilarghezza_alta
    w = semilarghezza
    v = [(x0, y - w, z0), (x1, y - w, z0), (x1, y + w, z0), (x0, y + w, z0),
         (xa, y - wa, z1), (xb, y - wa, z1), (xb, y + wa, z1), (xa, y + wa, z1)]
    f = [(4, 5, 6, 7), (0, 1, 5, 4), (1, 2, 6, 5), (2, 3, 7, 6), (3, 0, 4, 7), (0, 3, 2, 1)]
    tri = []
    for a, b, c, d in f: tri += _quad(v[a], v[b], v[c], v[d])
    return _mesh(tri, mat)


def poppa_slanciata(x_galleggiamento, x_coperta, z_coperta, semilarghezza, mat='scafo'):
    """Cuneo dello specchio di poppa inclinato: lo scafo del kit termina con uno specchio verticale a
    x = x_galleggiamento; il cuneo va dal galleggiamento (z = 0) in quel punto fino allo spigolo superiore
    dello specchio, arretrato a x_coperta (< x_galleggiamento) alla quota z_coperta."""
    prof = [(x_galleggiamento, 0.0), (x_galleggiamento, z_coperta), (x_coperta, z_coperta)]
    m = prisma_xz(prof, -semilarghezza, semilarghezza, mat)
    # coperta del cuneo (piano di volo) in materiale ponte
    m = m + scatola(x_coperta, x_galleggiamento, -semilarghezza, semilarghezza, z_coperta - 0.05, z_coperta, 'ponte')
    return m


def top_pair(x, z0, altezza_colonna=3.0, r_colonna=0.9, orientamento=90.0, y=0.0):
    """Radar Top Pair: colonna e due riflettori a graticcio schiena contro schiena. Con orientamento 0
    il Top Sail (grande) guarda verso prua e il Big Net verso poppa; ruotato di `orientamento` gradi."""
    m = cilindro(x, y, r_colonna, z0, z0 + altezza_colonna, 'sovrastruttura', n=10)
    zc = z0 + altezza_colonna
    # i riflettori sono graticci quasi trasparenti: resi come cornici e sbarre (kit.antenna_radar, piano y-z
    # con faccia verso +x), non come superfici piene
    # Top Sail: riflettore grande, 7,5 x 6,5 m, a 1,6 m davanti all'asse della colonna
    ts = kit.antenna_radar(x + 1.6, zc + 0.4, 7.5, 6.5, y=y, sbarre=6, mat='traliccio', spessore=0.25)
    # Big Net: riflettore ovale piu' piccolo, 6 x 3,5 m, sul lato opposto e un po' piu' basso
    bn = kit.antenna_radar(x - 1.4, zc + 0.9, 6.0, 3.5, y=y, sbarre=4, mat='traliccio', spessore=0.25)
    sostegno = scatola(x - 1.4, x + 1.6, y - 0.4, y + 0.4, zc, zc + 5.5, 'traliccio')
    return m + (ts + bn + sostegno).ruota_z(orientamento, x, y)


def lanciatore_binato(x, z0, y=0.0, altezza=2.4, braccio=3.4, interasse=1.6, verso=1.0, r=1.4):
    """Lanciatore a due bracci su piedistallo cilindrico (SS-N-14 del Kirov): bracci paralleli verso prua."""
    m = cilindro(x, y, r, z0, z0 + altezza * 0.55, n=12)
    for s in (-1, 1):
        m = m + scatola(x - braccio / 2 * 0.3, x + braccio / 2, y + s * interasse / 2 - 0.3, y + s * interasse / 2 + 0.3,
                        z0 + altezza * 0.55, z0 + altezza)
    if verso < 0:
        m = kit._rifletti_x(m, x)
    return m


PARTI = dict(tronco=tronco, poppa_slanciata=poppa_slanciata, top_pair=top_pair, lanciatore_binato=lanciatore_binato)
