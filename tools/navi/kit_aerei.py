"""Modulo del kit: parti per aerei (dal lotto del 23/10/2026: P-3C, Il-38, Tu-142).
(Nato come tools/navi/classi/aerei_parti.py; registrato in kit.PARTI.)
"""
"""Parti per i velivoli da pattugliamento marittimo (P-3C, Il-38, Tu-142M): oggetti in volo.

Coordinate del kit: x verso il muso (origine a meta' lunghezza fuori tutto), y verso SINISTRA, z verso l'alto.
Il modello sta tutto sopra z = 0 (asse della fusoliera a una quota z, per esempio 10 m).
Nel YAML le x si possono dare come {prua: d}, distanza dal muso (x = L/2 - d), anche dentro le liste.

Parti:
  fusoliera  corpo di rivoluzione dal muso alla coda con profilo tabulato; usabile come generatore dello scafo
             (`scafo: {generatore: fusoliera, L: ..., profilo: ..., z: ...}`), riceve e ignora B e bordo;
  corpo      corpo di rivoluzione generico lungo x (gondole, boma MAD, radome, ogive, carenature);
  ala        pannello alare rastremato e a freccia, con diedro (dalle quote di radice ed estremita'), profilo
             NACA simmetrico; `simmetrica` aggiunge il pannello opposto; `verticale` per derive;
  elica      disco dell'elica in movimento (materiale 'traliccio': sfocato come nelle foto), anche doppio
             (eliche controrotanti), con ogiva facoltativa;
  ellissoide radome e carenature ellissoidali.
"""
import sys
sys.path.insert(0, '/home/randolph/Claude/melchior-688/tools/navi')
import numpy as np
import kit
from kit import Mesh


def _sezioni(profilo):
    """Profilo [[d, r] | [d, w, h] | [d, w, h, dz], ...] -> array (n, 4) con d, semilarghezza, semialtezza, dz."""
    out = []
    for p in profilo:
        p = [float(v) for v in p]
        if len(p) == 2: out.append([p[0], p[1], p[1], 0.0])
        elif len(p) == 3: out.append([p[0], p[1], p[2], 0.0])
        else: out.append(p[:4])
    return np.array(out)


def corpo(x_naso, profilo, y=0.0, z=0.0, verso=-1.0, n=16, mat='sovrastruttura', simmetrico=False):
    """Corpo di rivoluzione (sezioni ellittiche) lungo x. `profilo`: [[d, r], ...] oppure [[d, w, h], ...] o
    [[d, w, h, dz], ...] con d = distanza dalla punta (x = x_naso + verso * d), w e h semilarghezza e
    semialtezza, dz spostamento verticale del centro della sezione (code rialzate, musi abbassati).
    Le estremita' con raggio non nullo vengono chiuse. `simmetrico`: anche la copia in -y."""
    S = _sezioni(profilo)
    a = 2 * np.pi * np.arange(n) / n
    anelli = [np.stack([np.full(n, x_naso + verso * d), y + w * np.cos(a), z + dz + h * np.sin(a)], 1)
              for d, w, h, dz in S]
    tri = []
    for A, B in zip(anelli[:-1], anelli[1:]):
        for k in range(n):
            k1 = (k + 1) % n
            tri += [(A[k], B[k], B[k1]), (A[k], B[k1], A[k1])]
    for (d, w, h, dz), R in ((S[0], anelli[0]), (S[-1], anelli[-1])):
        if w > 1e-6 or h > 1e-6:
            c = np.array([x_naso + verso * d, y, z + dz])
            tri += [(c, R[k], R[(k + 1) % n]) for k in range(n)]
    m = Mesh(np.array(tri), [mat] * len(tri))
    if simmetrico and abs(y) > 1e-9:
        t = m.tri.copy(); t[..., 1] *= -1
        m = m + Mesh(t, m.mat)
    return m


def fusoliera(L, profilo, z=10.0, n=20, mat='sovrastruttura', **_):
    """Fusoliera dal muso (x = L/2) verso la coda; `profilo` come in `corpo` (d dal muso). Come generatore dello
    scafo riceve anche B e bordo (ignorati)."""
    return corpo(L / 2.0, profilo, 0.0, z, -1.0, n, mat)


def _profilo_alare():
    """Frazioni di corda e semispessore relativo (NACA 00xx normalizzato a 1 allo spessore massimo)."""
    f = np.array([0.0, 0.02, 0.06, 0.13, 0.25, 0.4, 0.55, 0.7, 0.85, 1.0])
    t = 5 * (0.2969 * np.sqrt(f) - 0.126 * f - 0.3516 * f ** 2 + 0.2843 * f ** 3 - 0.1036 * f ** 4) / 0.5
    return f, t


def ala(radice, estremita, spessore=0.13, spessore_estremita=None, simmetrica=True, verticale=False,
        mat='sovrastruttura', sezioni=None):
    """Pannello alare tra due sezioni. `radice`, `estremita`: [x del bordo d'attacco, y, z, corda] (la x si puo'
    dare come {prua: d}). La corda va da x verso -x (verso la coda). Spessore relativo alla corda (alla radice e
    all'estremita'). Il diedro viene dalle quote; la freccia dalle x. `verticale`: spessore lungo y (derive).
    `sezioni`: sezioni intermedie [[x, y, z, corda], ...] tra radice ed estremita' (pannelli a piu' tratti).
    `simmetrica`: aggiunge il pannello speculare (y -> -y)."""
    se = spessore if spessore_estremita is None else spessore_estremita
    sez = [radice] + list(sezioni or []) + [estremita]
    sez = [[float(v) for v in s] for s in sez]
    # spessore interpolato lungo l'apertura (distanza dalla radice)
    P = np.array([s[:3] for s in sez]); dist = np.r_[0, np.cumsum(np.linalg.norm(np.diff(P, axis=0), axis=1))]
    sp = spessore + (se - spessore) * dist / max(dist[-1], 1e-9)
    f, t = _profilo_alare()
    def contorno(s, tk):
        x, y, z, c = s
        xs = x - f * c; h = 0.5 * tk * c * t
        if verticale:
            sup = np.stack([xs, y + h, np.full_like(xs, z)], 1); inf = np.stack([xs, y - h, np.full_like(xs, z)], 1)
        else:
            sup = np.stack([xs, np.full_like(xs, y), z + h], 1); inf = np.stack([xs, np.full_like(xs, y), z - h], 1)
        return np.concatenate([sup, inf[::-1][1:-1]])       # anello chiuso: dorso dal b.a. al b.u., ventre a ritroso
    anelli = [contorno(s, k) for s, k in zip(sez, sp)]
    m_ = len(anelli[0]); tri = []
    for A, B in zip(anelli[:-1], anelli[1:]):
        for k in range(m_):
            k1 = (k + 1) % m_
            tri += [(A[k], B[k], B[k1]), (A[k], B[k1], A[k1])]
    for R in (anelli[0], anelli[-1]):
        c = R.mean(0); tri += [(c, R[k], R[(k + 1) % m_]) for k in range(m_)]
    m = Mesh(np.array(tri), [mat] * len(tri))
    if simmetrica:
        tt = m.tri.copy(); tt[..., 1] *= -1
        m = m + Mesh(tt, m.mat)
    return m


def elica(x, y, z, r, simmetrica=True, mat='retinato', n=24, spessore=0.06, doppia=0.0, ogiva=None,
          mat_ogiva='sovrastruttura'):
    """Disco dell'elica in rotazione nel piano y-z alla x data, nel materiale 'retinato' (semitrasparente a
    scacchiera, senza tessitura dalle foto; fino al 23/10/2026 era 'traliccio', opaco).
    `doppia`: distanza (m) del secondo disco, verso la coda (eliche controrotanti). `ogiva`: [lunghezza, raggio]
    dell'ogiva conica davanti al disco. `simmetrica`: anche il motore speculare in -y."""
    a = 2 * np.pi * np.arange(n) / n
    def disco(x0):
        tri = []
        for xx in (x0 + spessore / 2, x0 - spessore / 2):
            R = np.stack([np.full(n, xx), y + r * np.cos(a), z + r * np.sin(a)], 1); c = np.array([xx, y, z])
            tri += [(c, R[k], R[(k + 1) % n]) for k in range(n)]
        return tri
    tri = disco(x) + (disco(x - doppia) if doppia else [])
    m = Mesh(np.array(tri), [mat] * len(tri))
    if ogiva:
        lo, ro = ogiva
        m = m + corpo(x + lo, [[0, 0], [lo * 0.5, ro * 0.8], [lo, ro], [lo + max(doppia, 0.2), ro]], y, z, n=12,
                      mat=mat_ogiva)
    if simmetrica and abs(y) > 1e-9:
        t = m.tri.copy(); t[..., 1] *= -1
        m = m + Mesh(t, m.mat)
    return m


def ellissoide(x, y, z, a, b, c, mat='sovrastruttura', n=14, simmetrico=False):
    """Ellissoide di semiassi a (lungo x), b (y), c (z) centrato in (x, y, z): radome, carenature."""
    d = a * (1 - np.cos(np.linspace(0, np.pi, 9)))
    s = np.sqrt(np.clip(1 - ((d - a) / a) ** 2, 0, 1))
    return corpo(x + a, [[di, b * si, c * si] for di, si in zip(d, s)], y, z, n=n, mat=mat, simmetrico=simmetrico)


PARTI = dict(fusoliera=fusoliera, corpo=corpo, ala=ala, elica=elica, ellissoide=ellissoide)
