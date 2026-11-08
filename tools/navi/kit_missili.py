"""Modulo del kit: parti per missili e bersagli (dal lotto del 23/10/2026: Tomahawk, Harpoon, P-700, BQM-74).
(Nato come tools/navi/classi/missili_parti.py; registrato in kit.PARTI.)
"""
"""Parti per missili e bersagli aerei (Tomahawk, Harpoon, P-700 Granit, BQM-74C Chukar).

Coordinate del kit: x verso la prua (il muso), y verso sinistra, z verso l'alto, metri. Gli oggetti in volo
stanno tutti sopra z = 0: l'asse del corpo e' a quota `z` (predefinita 5 m).

Parti:
  corpo_missile  generatore dello scafo (scafo: {generatore: corpo_missile, ...}): corpo di rivoluzione
                 con raggio tabulato lungo la lunghezza, misurato dal muso; sezione eventualmente ellittica.
  pinna          superficie piana trapezia (ala, aletta, deriva) attaccata al corpo, a un angolo di rollio
                 qualsiasi: 0 = verso l'alto, 90 = verso sinistra (+y), 180 = verso il basso, 270 = verso dritta.
  pinne          piu' pinne uguali a diversi angoli (croce '+' = [0, 90, 180, 270], croce 'X' = [45, 135, 225, 315]).
  presa_ventrale presa d'aria a cucchiaio sotto il corpo (profilo x-z estruso), bocca anteriore scura.
  gondola        corpo di rivoluzione secondario parallelo all'asse (gondola del motore), raggio tabulato.
  anello         corona circolare piana perpendicolare all'asse (bocca scura di una presa d'aria anulare).
"""
import sys
sys.path.insert(0, '/home/randolph/Claude/melchior-688/tools/navi')
import numpy as np
import kit


def _anelli(xs, rs, y, z, n, ellitticita=1.0):
    """Vertici degli anelli di un corpo di rivoluzione: (len(xs), n, 3). ellitticita = semiasse z / semiasse y."""
    a = 2 * np.pi * np.arange(n) / n
    ca, sa = np.cos(a), np.sin(a)
    V = np.zeros((len(xs), n, 3))
    for i, (x, r) in enumerate(zip(xs, rs)):
        V[i, :, 0] = x
        V[i, :, 1] = y + r * ca
        V[i, :, 2] = z + r * ellitticita * sa
    return V


def _rivoluzione(xs, rs, y, z, n, mat, ellitticita=1.0):
    V = _anelli(xs, rs, y, z, n, ellitticita)
    tri = []
    for i in range(len(xs) - 1):
        for k in range(n):
            a, b = V[i, k], V[i, (k + 1) % n]
            c, d = V[i + 1, (k + 1) % n], V[i + 1, k]
            if rs[i] <= 1e-6:                      # punta: un solo triangolo
                tri.append((a, c, d))
            elif rs[i + 1] <= 1e-6:
                tri.append((a, b, c))
            else:
                tri += kit._quad(a, b, c, d)
    # tappi alle estremita' con raggio non nullo (a ventaglio sull'asse)
    for i in (0, len(xs) - 1):
        if rs[i] > 1e-6:
            centro = np.array([xs[i], y, z])
            for k in range(n):
                tri.append((centro, V[i, k], V[i, (k + 1) % n]))
    return kit._mesh(tri, mat)


def corpo_missile(L, profilo, z=5.0, y=0.0, n=16, mat='sovrastruttura', ellitticita=1.0, **_ignora):
    """Corpo di rivoluzione lungo L (generatore dello scafo). profilo: [[d, r], ...] con d distanza dal muso
    (0..L) e r raggio in metri; i punti si ordinano dal muso alla coda. Gli altri parametri della sezione
    `scafo` (B, bordo, ...) sono ignorati: servono solo al formato della classe."""
    p = sorted((float(d), float(r)) for d, r in profilo)
    xs = [L / 2 - d for d, _ in p]; rs = [r for _, r in p]
    return _rivoluzione(xs, rs, y, z, n, mat, ellitticita)


def gondola(profilo, y=0.0, z=5.0, n=12, mat='sovrastruttura', ellitticita=1.0):
    """Corpo di rivoluzione secondario parallelo all'asse x. profilo: [[x, r], ...] in coordinate del modello
    (x verso il muso; nel YAML si puo' usare {prua: d})."""
    p = sorted(((float(x), float(r)) for x, r in profilo), reverse=True)
    return _rivoluzione([q[0] for q in p], [q[1] for q in p], y, z, n, mat, ellitticita)


def pinna(x_ba, corda, x_ba_estremo, corda_estremo, r0, semiapertura, angolo, z=5.0, y=0.0, spessore=0.03,
          mat='sovrastruttura', dz=0.0):
    """Superficie piana trapezia. Radice a distanza r0 dall'asse (bordo d'attacco x_ba, corda `corda`),
    estremita' a distanza `semiapertura` dall'asse (bordo d'attacco x_ba_estremo, corda `corda_estremo`).
    `angolo` di rollio in gradi: 0 = in alto, 90 = a sinistra (+y), 180 = in basso, 270 = a dritta.
    `dz`: spostamento verticale del piano della pinna rispetto all'asse (ali alte o basse)."""
    a = np.radians(angolo)
    d = np.array([0.0, np.sin(a), np.cos(a)])            # verso l'estremita'
    nrm = np.array([0.0, np.cos(a), -np.sin(a)])          # normale al piano della pinna
    o = np.array([0.0, y, z + dz])
    h = spessore / 2
    P = [o + np.array([x_ba, 0, 0]) + r0 * d, o + np.array([x_ba - corda, 0, 0]) + r0 * d,
         o + np.array([x_ba_estremo - corda_estremo, 0, 0]) + semiapertura * d,
         o + np.array([x_ba_estremo, 0, 0]) + semiapertura * d]
    v = [p - h * nrm for p in P] + [p + h * nrm for p in P]
    f = [(0, 1, 2, 3), (4, 7, 6, 5), (0, 4, 5, 1), (1, 5, 6, 2), (2, 6, 7, 3), (3, 7, 4, 0)]
    tri = []
    for i, j, k, l in f: tri += kit._quad(v[i], v[j], v[k], v[l])
    return kit._mesh(tri, mat)


def pinne(x_ba, corda, x_ba_estremo, corda_estremo, r0, semiapertura, angoli, z=5.0, y=0.0, spessore=0.03,
          mat='sovrastruttura', dz=0.0):
    """Piu' pinne uguali (alette cruciformi, coppia d'ali) agli angoli di rollio dati."""
    m = kit.Mesh()
    for a in angoli:
        m = m + pinna(x_ba, corda, x_ba_estremo, corda_estremo, r0, semiapertura, a, z, y, spessore, mat, dz)
    return m


def presa_ventrale(x_bocca, x_coda, larghezza, sporgenza, z_attacco, lunghezza_bocca=None, mat='sovrastruttura',
                   mat_bocca='nero', y=0.0):
    """Presa d'aria a cucchiaio sotto il corpo: bocca verticale a x_bocca, fondo che sporge di `sporgenza` sotto
    `z_attacco` (quota del ventre del corpo, leggermente dentro il corpo) e risale fino a x_coda.
    `lunghezza_bocca`: tratto a sporgenza piena dietro la bocca (predefinito un terzo della lunghezza)."""
    lb = (x_bocca - x_coda) / 3 if lunghezza_bocca is None else lunghezza_bocca
    zb = z_attacco - sporgenza
    prof = [(x_bocca, z_attacco), (x_bocca, zb), (x_bocca - lb, zb), (x_coda, z_attacco)]
    m = kit.prisma_xz(prof, y - larghezza / 2, y + larghezza / 2, mat)
    # bocca: lamina scura appena davanti alla faccia anteriore
    e = 0.004
    bocca = kit.scatola(x_bocca + e, x_bocca + 2 * e, y - larghezza / 2 * 0.8, y + larghezza / 2 * 0.8,
                        zb + sporgenza * 0.15, z_attacco - sporgenza * 0.05, mat=mat_bocca)
    return m + bocca


def anello(x, r_interno, r_esterno, z=5.0, y=0.0, n=24, mat='nero'):
    """Corona circolare piana nel piano x = costante (es. la presa d'aria anulare del P-700 attorno al cono
    centrale), con le due facce."""
    a = 2 * np.pi * np.arange(n + 1) / n
    P = lambda r, t: np.array([x, y + r * np.cos(t), z + r * np.sin(t)])
    tri = []
    for k in range(n):
        tri += kit._quad(P(r_interno, a[k]), P(r_esterno, a[k]), P(r_esterno, a[k + 1]), P(r_interno, a[k + 1]))
    return kit._mesh(tri, mat)


PARTI = dict(anello=anello, corpo_missile=corpo_missile, gondola=gondola, pinna=pinna, pinne=pinne, presa_ventrale=presa_ventrale)
