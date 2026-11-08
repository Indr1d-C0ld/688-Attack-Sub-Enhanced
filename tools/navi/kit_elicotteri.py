"""Modulo del kit: parti per elicotteri (dal lotto del 23/10/2026: SH-60B, Ka-25).
(Nato come tools/navi/classi/elicotteri_parti.py; registrato in kit.PARTI.)
"""
"""Parti per gli elicotteri (SH-60B Seahawk, Ka-25PL Hormone-A), configurazione 1989.

Coordinate del kit (destrorse, metri): x verso prua, y verso SINISTRA, z verso l'alto. Gli elicotteri sono
oggetti in volo (`vista: {volante: true}`): il modello sta tutto sopra z = 0. Le parti ricevono x gia'
risolte (i valori {prua: d} sono convertiti da costruisci.py, anche dentro le liste), tranne il
generatore `fusoliera`, che riceve i parametri della sezione `scafo` cosi' come sono (distanze dalla prua).

Parti:
  fusoliera     fusoliera a sezioni (superellissi) interpolate tra stazioni; generatore dello "scafo"
                (scafo: {generatore: fusoliera, ...}) oppure parte qualsiasi (gondole, carenature)
  piano         lastra a pianta poligonale (x, y) con spessore, ruotata in beccheggio: stabilizzatori, piani di coda
  deriva_y      lastra verticale a profilo poligonale (x, z), spessa e inclinata attorno all'asse x: derive
  ellissoide    ellissoide (radome, serbatoi, carenature tondeggianti)
  ruota         ruota con l'asse lungo y
  tubo_inclinato cilindro tra due punti qualsiasi (siluri, gambe del carrello, scarichi)
  disco_rotore  rotore in moto come disco "puntinato": tanti quadratini sparsi nel disco, nel materiale
                'scuro', con densita' data. Una lamina piena coprirebbe la fusoliera come un'ellisse grigia;
                il puntinato riproduce nei 16 grigi della palette la trasparenza del disco sfocato delle foto
                (e la retinatura degli sprite originali del 1989). Conicita' delle pale e normale qualsiasi
                (rotore di coda inclinato).
"""
import sys
sys.path.insert(0, '/home/randolph/Claude/melchior-688/tools/navi')
import numpy as np
import kit
from kit import Mesh

def _quad(a, b, c, d):
    return [(a, b, c), (a, c, d)]

def _sezione(zb, zt, w, p, n, yc=0.0):
    """Punti (y, z) di una superellisse |y/w|^p + |(z-zc)/h|^p = 1, da sotto in senso antiorario."""
    zc, h = (zb + zt) / 2, (zt - zb) / 2
    a = -np.pi / 2 + 2 * np.pi * np.arange(n) / n
    c, s = np.cos(a), np.sin(a)
    y = w * np.sign(c) * np.abs(c) ** (2 / p); z = h * np.sign(s) * np.abs(s) ** (2 / p)
    return np.stack([yc + y, zc + z], 1)

def fusoliera(L, sezioni, n=20, mat='sovrastruttura', z0=0.0, y0=0.0, dx=0.0, **altri):
    """Fusoliera interpolata tra stazioni `sezioni` = [[d, z_basso, z_alto, semilarghezza, esponente], ...]
    (d = distanza dalla prua, x = L/2 - d + dx; esponente 2 = ellisse, 3-4 = rettangolo arrotondato;
    facoltativo un sesto valore = spostamento laterale del centro). Estremita' chiuse con tappi.
    Gli altri parametri della sezione `scafo` (es. B, bordo) sono ignorati."""
    S = [list(map(float, s)) for s in sorted(sezioni, key=lambda s: s[0])]
    anelli = []
    for s in S:
        d, zb, zt, w = s[:4]; p = s[4] if len(s) > 4 else 2.0; yc = s[5] if len(s) > 5 else 0.0
        yz = _sezione(zb + z0, zt + z0, max(w, 1e-3), p, n, yc + y0)
        x = L / 2 - d + dx
        anelli.append([(x, y, z) for y, z in yz])
    tri = []
    for A, B in zip(anelli[:-1], anelli[1:]):
        for k in range(n):
            tri += _quad(A[k], A[(k + 1) % n], B[(k + 1) % n], B[k])
    for A in (anelli[0], anelli[-1]):
        c = tuple(np.mean(A, 0))
        tri += [(c, A[k], A[(k + 1) % n]) for k in range(n)]
    return Mesh(tri, [mat] * len(tri))

def piano(profilo, z, spessore=0.1, beccheggio=0.0, asse_x=None, rollio=0.0, asse_y=0.0, mat='sovrastruttura'):
    """Lastra a pianta convessa `profilo` = [[x, y], ...] (in ordine), quota media z, spessore dato.
    `beccheggio` (gradi, positivo = bordo d'attacco in alto) attorno all'asse parallelo a y per (asse_x, z)
    (predefinito: centro del profilo); `rollio` (gradi) attorno all'asse parallelo a x per (asse_y, z)."""
    p = [tuple(map(float, q)) for q in profilo]; n = len(p); h = spessore / 2; tri = []
    for i in range(n):
        (xa, ya), (xb, yb) = p[i], p[(i + 1) % n]
        tri += _quad((xa, ya, z - h), (xb, yb, z - h), (xb, yb, z + h), (xa, ya, z + h))
    for k in range(1, n - 1):
        for zz in (z - h, z + h):
            tri.append(((p[0][0], p[0][1], zz), (p[k][0], p[k][1], zz), (p[k + 1][0], p[k + 1][1], zz)))
    m = Mesh(tri, [mat] * len(tri))
    if rollio:
        a = np.radians(rollio); c, s = np.cos(a), np.sin(a)
        t = m.tri.copy(); yy, zz = t[..., 1] - asse_y, t[..., 2] - z
        t[..., 1], t[..., 2] = asse_y + c * yy - s * zz, z + s * yy + c * zz
        m = Mesh(t, m.mat)
    if beccheggio:
        cx = float(np.mean([q[0] for q in p])) if asse_x is None else asse_x
        m = m.ruota_y(beccheggio, cx, z)
    return m

def deriva_y(profilo, y, spessore=0.25, inclinazione=0.0, mat='sovrastruttura'):
    """Lastra verticale a profilo convesso `profilo` = [[x, z], ...] centrata in y, spessa `spessore`;
    `inclinazione` (gradi) la ruota attorno all'asse x passante per la base (punto piu' basso del profilo):
    positivo = sommita' verso sinistra (+y)."""
    m = kit.prisma_xz(profilo, y - spessore / 2, y + spessore / 2, mat)
    if inclinazione:
        z0 = min(q[1] for q in profilo); a = np.radians(inclinazione); c, s = np.cos(a), np.sin(a)
        t = m.tri.copy(); yy, zz = t[..., 1] - y, t[..., 2] - z0
        t[..., 1], t[..., 2] = y + c * yy - s * zz, z0 + s * yy + c * zz
        m = Mesh(t, m.mat)
    return m

def ellissoide(x, y, z, rx, ry, rz, mat='sovrastruttura', n=12):
    m = kit.sfera(0, 0, 0, 1.0, mat, n)
    t = m.tri * np.array([rx, ry, rz]) + np.array([x, y, z])
    return Mesh(t, m.mat)

def ruota(x, y, z, r, larghezza=0.2, mat='nero', n=12):
    """Ruota (cilindro con l'asse lungo y) centrata in (x, y, z)."""
    m = kit.cilindro(0, 0, r, -larghezza / 2, larghezza / 2, mat, n)
    t = m.tri.copy(); t = t[..., [0, 2, 1]]           # asse z -> asse y
    return Mesh(t + np.array([x, y, z]), m.mat)

def _base_ortonormale(nv):
    nv = np.asarray(nv, float); nv = nv / np.linalg.norm(nv)
    a = np.array([1.0, 0, 0]) if abs(nv[0]) < 0.9 else np.array([0, 1.0, 0])
    e1 = np.cross(nv, a); e1 /= np.linalg.norm(e1); e2 = np.cross(nv, e1)
    return nv, e1, e2

def tubo_inclinato(da, a, r, mat='sovrastruttura', n=10, r_fine=None):
    """Cilindro (o tronco di cono con `r_fine`) tra i punti `da` e `a`, con i tappi."""
    p0, p1 = np.asarray(da, float), np.asarray(a, float)
    ax, e1, e2 = _base_ortonormale(p1 - p0); r1 = r if r_fine is None else r_fine
    ang = 2 * np.pi * np.arange(n) / n
    A = [p0 + r * (np.cos(t) * e1 + np.sin(t) * e2) for t in ang]
    B = [p1 + r1 * (np.cos(t) * e1 + np.sin(t) * e2) for t in ang]
    tri = []
    for k in range(n):
        tri += _quad(A[k], A[(k + 1) % n], B[(k + 1) % n], B[k])
        tri += [(p0, A[(k + 1) % n], A[k]), (p1, B[k], B[(k + 1) % n])]
    return Mesh(tri, [mat] * len(tri))

def disco_rotore(x, y, z, raggio, normale=(0.0, 0.0, 1.0), conicita=3.0, densita=0.3, lato=0.12,
                 raggio_interno=0.4, seme=1, mat='scuro', spessore=0.0, inclinazione=0.0):
    """Rotore in moto: quadratini di lato `lato` sparsi a caso (seme fisso, quindi ripetibile) nella corona tra
    `raggio_interno` e `raggio`, con frazione d'area coperta `densita`. Il disco e' perpendicolare a `normale` e
    le pale salgono di `conicita` gradi verso l'esterno (cono di volo). A ogni angolo di vista la frazione di
    campioni che colpisce il disco resta circa `densita`: negli sprite il disco appare come una retinatura rada,
    fitta dove il disco e' visto di taglio. `spessore` (m) sparge i quadratini lungo la normale e `inclinazione`
    (gradi) li inclina a caso fino a quel valore: senza, visto esattamente di taglio (elevazione 0) il disco piatto
    sfuggirebbe quasi del tutto alla soglia di copertura degli sprite."""
    rng = np.random.default_rng(seme)
    nv, e1, e2 = _base_ortonormale(normale)
    area = np.pi * (raggio ** 2 - raggio_interno ** 2)
    N = int(densita * area / lato ** 2)
    rr = np.sqrt(rng.uniform(raggio_interno ** 2, raggio ** 2, N)); th = rng.uniform(0, 2 * np.pi, N)
    rot = rng.uniform(0, np.pi / 2, N)
    dz = rng.uniform(-spessore / 2, spessore / 2, N) if spessore else np.zeros(N)
    inc = np.radians(rng.uniform(-inclinazione, inclinazione, N)) if inclinazione else np.zeros(N)
    tc = np.tan(np.radians(conicita))
    C = np.array([x, y, z], float)
    tri = []
    h = lato / 2
    for r_, t_, g, h_, i_ in zip(rr, th, rot, dz, inc):
        u = np.cos(t_) * e1 + np.sin(t_) * e2           # direzione radiale
        P = C + r_ * u + (r_ * tc + h_) * nv
        a = np.cos(g) * e1 + np.sin(g) * e2; b = np.cross(nv, a)
        b = np.cos(i_) * b + np.sin(i_) * nv             # quadratino inclinato attorno ad a
        v = [P + h * (sa * a + sb * b) for sa, sb in ((-1, -1), (1, -1), (1, 1), (-1, 1))]
        tri += _quad(*v)
    return Mesh(tri, [mat] * len(tri))

PARTI = dict(fusoliera=fusoliera, piano=piano, deriva_y=deriva_y, ellissoide=ellissoide, ruota=ruota,
             tubo_inclinato=tubo_inclinato, disco_rotore=disco_rotore)
