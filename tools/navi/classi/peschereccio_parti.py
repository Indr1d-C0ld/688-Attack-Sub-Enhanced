"""Parti proprie della classe "peschereccio" (peschereccio d'altura a rampa poppiera, tipo norvegese degli anni '80,
modello sull'Atlantic Peace del 1987, 57 x 13 m). Coordinate del kit (x verso prua, y verso sinistra, z in alto,
metri); i valori {prua: d} del YAML arrivano gia' convertiti in x.

- portale_poppiero: portale (gantry) a poppa per salpare la rete: due gambe a cassone inclinate verso l'asse (a
  "lambda" viste da poppa), ciascuna su una cassa sul ponte di lavoro, traversa in cima con la piattaforma,
  alberetto con la frusta e il tubo di scarico del motore che esce dalla traversa (sull'Atlantic Peace lo scarico
  passa nel portale).
- tamburo_rete: tamburo avvolgirete ad asse trasversale (lungo y) con le due flange.
- gru_articolata: gru di coperta a braccio snodato: colonna, primo braccio alzato, secondo braccio ripiegato.
- rampa_poppiera: rampa di poppa (slip) vista da fuori: apertura scura nello specchio, dal piede della rampa al
  ponte di lavoro, tra due spalle chiare.
- palo_carico: struttura a "pi greco" all'estremita' poppiera della sovrastruttura (due montanti e traversa),
  per i bozzelli delle manovre della rete.
- pannello_specchio: rivestimento dello specchio con materiale a scelta (come per la Nanuchka): con 'scafo'
  diventa un blocco quasi nero nelle viste di poppa che le foto non coprono.

Le parti sottili (frusta, sartie) usano i materiali 'traliccio' e 'scuro'.
"""
import sys
sys.path.insert(0, '/home/randolph/Claude/melchior-688/tools/navi')
import numpy as np
import kit
from kit import Mesh, asta, scatola, cilindro, tubo


def _gamba(base, cima, lato_x, lato_y, mat):
    """Gamba a cassone (sezione rettangolare lato_x x lato_y) dal centro della base al centro della cima."""
    (xb, yb, zb), (xc, yc, zc) = base, cima
    v = [(xb - lato_x / 2, yb - lato_y / 2, zb), (xb + lato_x / 2, yb - lato_y / 2, zb),
         (xb + lato_x / 2, yb + lato_y / 2, zb), (xb - lato_x / 2, yb + lato_y / 2, zb),
         (xc - lato_x / 2, yc - lato_y / 2, zc), (xc + lato_x / 2, yc - lato_y / 2, zc),
         (xc + lato_x / 2, yc + lato_y / 2, zc), (xc - lato_x / 2, yc + lato_y / 2, zc)]
    f = [(4, 5, 6, 7), (0, 1, 5, 4), (1, 2, 6, 5), (2, 3, 7, 6), (3, 0, 4, 7), (0, 3, 2, 1)]
    tri = []
    for a, b, c, d in f: tri += kit._quad(v[a], v[b], v[c], v[d])
    return Mesh(tri, [mat] * len(tri))


def portale_poppiero(x, z0, z_casse, z_traversa, x_base=None, x_casse=None, y_base=5.0, y_cima=3.9, casse=(2.9, 2.6),
                     lato_x=1.0, lato_y=1.1, traversa=1.4, z_albero=18.7, z_frusta=22.2, scarico=True, mat='bianco',
                     mat_casse='scafo', mat_traversa='scuro'):
    """Portale: casse (lunghezza, larghezza) centrate in x_casse a y = +-y_base dal ponte z0 a z_casse, gambe dalla
    cima delle casse (x_base, y = +-y_base) alla traversa centrata in x (asse delle gambe a y = +-y_cima; la traversa
    finisce a y_cima + lato_y/2). x_base e x_casse predefiniti = x (gambe senza inclinazione longitudinale), traversa alta `traversa` con il cielo a z_traversa."""
    lc, wc = casse
    xb = x if x_base is None else x_base; xc = x if x_casse is None else x_casse
    m = Mesh()
    zt = z_traversa - traversa
    for s in (-1, 1):
        m = m + scatola(xc - lc / 2, xc + lc / 2, s * y_base - wc / 2, s * y_base + wc / 2, z0, z_casse, mat_casse)
        m = m + _gamba((xb, s * y_base, z_casse), (x, s * y_cima, zt), lato_x, lato_y, mat)
    # traversa e piattaforma
    m = m + scatola(x - lato_x / 2, x + lato_x / 2, -y_cima - lato_y / 2, y_cima + lato_y / 2, zt, z_traversa,
                    mat_traversa)
    # alberetto con fanali (sottile) e frusta
    m = m + asta((x, 0.0, z_traversa), (x, 0.0, z_albero), 0.45, 'scuro')
    m = m + kit.pennone(x, z_albero - 1.2, 1.0, spessore=0.2, mat='scuro')
    m = m + asta((x, 0.0, z_albero), (x, 0.0, z_frusta), 0.12, 'traliccio')
    if scarico:      # tubo di scarico ricurvo sopra la traversa, a dritta dell'asse
        m = m + cilindro(x, -1.6, 0.35, z_traversa, z_traversa + 1.0, 'nero', n=8)
    return m


def tamburo_rete(x, y, z0, lunghezza=4.0, r=1.2, r_flangia=1.6, mat='ponte', mat_flange='scafo'):
    """Tamburo ad asse trasversale centrato in (x, y), con il fondo a z0 (appoggiato al ponte su due ritti)."""
    zc = z0 + r_flangia + 0.2
    corpo = tubo(-lunghezza / 2, lunghezza / 2, 0.0, 0.0, r, mat, n=12)
    for s in (-1, 1):
        corpo = corpo + tubo(s * lunghezza / 2 - 0.15, s * lunghezza / 2 + 0.15, 0.0, 0.0, r_flangia, mat_flange, n=12)
    corpo = corpo.ruota_z(90.0).sposta(x, y, zc)
    ritti = Mesh()
    for s in (-1, 1):
        ritti = ritti + scatola(x - 0.6, x + 0.6, y + s * (lunghezza / 2 + 0.35) - 0.2, y + s * (lunghezza / 2 + 0.35) + 0.2,
                                z0, zc + 0.3, mat_flange)
    return corpo + ritti


def gru_articolata(x, y, z0, colonna=2.5, braccio1=(5.0, 50.0), braccio2=(4.5, -150.0), verso=-1.0,
                   r_colonna=0.5, spessore=0.6, mat='bianco'):
    """Gru a braccio snodato: colonna cilindrica da z0, primo braccio lungo braccio1[0] alzato di braccio1[1] gradi
    verso `verso` (+1 prua, -1 poppa), secondo braccio ripiegato di braccio2[1] gradi rispetto al primo."""
    zc = z0 + colonna
    m = cilindro(x, y, r_colonna, z0, zc, mat, n=10)
    m = m + scatola(x - 0.6, x + 0.6, y - 0.6, y + 0.6, zc, zc + 0.8, mat)
    a1 = np.radians(braccio1[1]); p0 = np.array([x, y, zc + 0.6])
    p1 = p0 + braccio1[0] * np.array([verso * np.cos(a1), 0.0, np.sin(a1)])
    a2 = a1 + np.radians(braccio2[1])
    p2 = p1 + braccio2[0] * np.array([verso * np.cos(a2), 0.0, np.sin(a2)])
    return m + asta(p0, p1, spessore, mat) + asta(p1, p2, spessore * 0.8, mat)


def rampa_poppiera(x_poppa, semilarghezza, z_basso, z_ponte, spalle=0.5, sporgenza=0.15, mat='scuro'):
    """Rampa di poppa vista da fuori: l'apertura nello specchio (pannello scuro largo 2*semilarghezza, dal piede
    della rampa z_basso al ponte di lavoro z_ponte) con le due spalle chiare ai lati. Le viste degli sprite sono
    quasi a livello del mare: il piano inclinato della rampa, dentro lo scafo, non si vedrebbe comunque."""
    w, x = semilarghezza, x_poppa - sporgenza
    m = scatola(x, x_poppa + 0.05, -w, w, z_basso, z_ponte, mat)
    for s in (-1, 1):
        m = m + scatola(x - 0.05, x_poppa + 0.05, s * (w + spalle / 2) - spalle / 2, s * (w + spalle / 2) + spalle / 2,
                        z_basso, z_ponte + 0.6, 'bianco')
    return m


def palo_carico(x, y, z0, z1, lato=0.8, traversa=0.9, mat='bianco'):
    """Struttura a pi greco: due montanti a y = +-y e una traversa in cima."""
    m = Mesh()
    for s in (-1, 1):
        m = m + scatola(x - lato / 2, x + lato / 2, s * y - lato / 2, s * y + lato / 2, z0, z1 - traversa, mat, rastrema=0.95)
    return m + scatola(x - lato * 0.7, x + lato * 0.7, -y - lato / 2, y + lato / 2, z1 - traversa, z1, mat)


def pannello_specchio(mat='bianco', scostamento=0.03, **sc):
    """Trapezio sullo specchio di poppa (x = -L/2 - scostamento), dal galleggiamento alla coperta."""
    m = kit.scafo(**sc); XB = sc['L'] / 2; x = -XB - scostamento
    z1 = float(m.quota_coperta(-XB)); w0 = float(m.semilarghezza(-XB, 0.0)); w1 = float(m.semilarghezza(-XB, z1))
    tri = [((x, -w0, 0.0), (x, w0, 0.0), (x, w1, z1)), ((x, -w0, 0.0), (x, w1, z1), (x, -w1, z1))]
    return Mesh(tri, [mat] * 2)


PARTI = dict(portale_poppiero=portale_poppiero, tamburo_rete=tamburo_rete, gru_articolata=gru_articolata,
             rampa_poppiera=rampa_poppiera, palo_carico=palo_carico, pannello_specchio=pannello_specchio)
