"""Parti proprie della classe Nanuchka (Progetto 1234 "Ovod").

- lanciatore_malakhit: lanciatore triplo KT-84 dei P-120 Malakhit (SS-N-9) sul fianco della
  sovrastruttura prodiera: basamento, tre contenitori cilindrici (due sovrapposti sul lato esterno,
  uno interno piu' basso) e testate di chiusura; il fascio e' inclinato verso prua (`elevazione`).
- band_stand: radome del radar di scoperta e designazione bersagli "Band Stand" (Titanit):
  basamento cilindrico con cupola emisferica.
- pannello_specchio: rivestimento dello specchio di poppa con un materiale piu' chiaro. Nessuna foto
  disponibile vede lo specchio: resta l'ombreggiatura del modello, che con il materiale 'scafo' e la luce
  di lato diventa un blocco nero nelle viste di poppa. Riceve i parametri dello scafo.
- chiusura_chiara: come kit.chiusura_dritto (superficie sotto il dritto slanciato) ma con un solo materiale
  a scelta: con 'bagnasciuga'/'scafo' il cuneo sotto il dritto, che nessuna foto vede di fronte, risultava
  quasi nero nelle viste di prua.
- impianto_chiaro: kit.impianto_binato (qui l'AK-257 da 57 mm) con materiale a scelta: la faccia poppiera
  della torre non e' vista da nessuna foto e con 'sovrastruttura' risultava un blocco nero a 120-180 gradi.
"""
import sys
sys.path.insert(0, '/home/randolph/Claude/melchior-688/tools/navi')
import kit
from kit import Mesh, scatola, tubo, cilindro, sfera


def lanciatore_malakhit(x0, x1, y_int, y_est, z0, elevazione=8.0, r=0.8, z_base=0.6):
    """Contenitori tra x0 (poppa) e x1 (prua), tra y_int e y_est (lato esterno, con segno), base al ponte z0.
    I contenitori ruotano attorno allo spigolo poppiero inferiore: la testata prodiera sale."""
    L = x1 - x0; s = 1.0 if y_est > y_int else -1.0
    ye = y_est - s * r                      # colonna esterna (due contenitori sovrapposti)
    yi = y_int + s * r                      # contenitore interno, piu' basso
    fascio = (tubo(0.0, L, ye, z_base + r, r) + tubo(0.0, L, ye, z_base + 3 * r, r) +
              tubo(0.0, L, yi, z_base + r * 1.6, r))
    # telaio che lega i contenitori (fianco piatto verso l'interno e cielo)
    fascio = fascio + scatola(0.3, L - 0.3, min(yi, ye), max(yi, ye), z_base + 0.2, z_base + 3.4 * r, rastrema=1.0)
    fascio = fascio.ruota_y(elevazione)     # rotazione attorno all'origine (spigolo poppiero): la prua sale
    base = scatola(0.0, L * 0.85, min(y_int, y_est), max(y_int, y_est), 0.0, z_base + 0.3)
    return (base + fascio).sposta(x0, 0.0, z0)


def band_stand(x, z0, r=1.65, altezza=2.9, y=0.0):
    """Radome Band Stand: cilindro alto `altezza` sopra z0 e cupola emisferica."""
    return cilindro(x, y, r, z0, z0 + altezza, 'bianco', n=18) + sfera(x, y, z0 + altezza, r, 'bianco', n=12, mezza=True)


def pannello_specchio(mat='bianco', scostamento=0.03, **sc):
    """Trapezio sullo specchio di poppa (x = -L/2 - scostamento), dal galleggiamento alla coperta."""
    m = kit.scafo(**sc); XB = sc['L'] / 2; x = -XB - scostamento
    z1 = float(m.quota_coperta(-XB)); w0 = float(m.semilarghezza(-XB, 0.0)); w1 = float(m.semilarghezza(-XB, z1))
    tri = [((x, -w0, 0.0), (x, w0, 0.0), (x, w1, z1)), ((x, -w0, 0.0), (x, w1, z1), (x, -w1, z1))]
    return Mesh(tri, [mat] * 2)


def chiusura_chiara(mat='bianco', **sc):
    m = kit.chiusura_dritto(**sc)
    return Mesh(m.tri, [mat] * len(m.tri))


def impianto_chiaro(mat='bianco', **kw):
    m = kit.impianto_binato(**kw)
    return Mesh(m.tri, [mat if x == 'sovrastruttura' else x for x in m.mat])


PARTI = dict(impianto_chiaro=impianto_chiaro, chiusura_chiara=chiusura_chiara, lanciatore_malakhit=lanciatore_malakhit, band_stand=band_stand, pannello_specchio=pannello_specchio)
