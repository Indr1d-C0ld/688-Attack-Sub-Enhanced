"""Kit di parti parametriche per le navi del gioco.

Sistema di coordinate DESTRORSO, in metri:
  x verso prua (origine a meta' lunghezza fuori tutto), y verso SINISTRA, z verso l'alto
  (z = 0 linea di galleggiamento). Dalla dritta (y < 0) la prua appare a destra.

Ogni parte restituisce una Mesh: triangoli (N, 3, 3) + materiale per triangolo + nome della parte.
Materiali: 'scafo', 'bagnasciuga', 'sovrastruttura', 'ponte', 'traliccio', 'bianco', 'nero', 'scuro'.
I materiali 'traliccio' e 'scuro' indicano strutture sottili o a graticcio.

Parti sovietiche: tubo, lanciatore_quadruplo (Rastrub/SS-N-14), lanciatore_scomparsa (Osa-M),
lanciarazzi_rbu (RBU-6000/1000), paraboloide, head_net (due riflettori contrapposti),
direttore_tiro (Pop Group, Owl Screech), torre_singola (AK-100), lanciasiluri_quadruplo,
tronco (tronco di piramide con fronte e retro indipendenti: torri e sovrastrutture), ak630, ak230 (binato
da 30 mm), ak130 (binato da 130 mm). Il Kirov ridefinisce `tronco` nel proprio modulo con un'altra firma.

Parti NATO e di tipo generale (nate con le classi Kidd, Iowa, Nimitz e Sacramento):
  armi: lanciatore_mk26, cannone_mk45, harpoon (Mk 141), abl (Mk 143), sea_sparrow (Mk 29),
        torre_trinata, impianto_binato;
  sensori e direzione del tiro: sps48, antenna_piana, direttore_mk37, direttore_mk38, direttore_mk91;
  strutture: mack, fumaiolo_cappello, fumaiolo_cilindrico, albero_palo, gru, stazioni_rifornimento,
        sponsone, prisma_xz (profilo x-z estruso), aereo, aerei;
  scafo: chiusura_dritto (chiude il dritto slanciato degli scafi larghi; riceve i parametri dello scafo),
        scafo_composto (poppa a incrociatore; generatore alternativo a `scafo`, vedi costruisci.py).
I moduli `moduli_parti` di una classe possono ridefinire queste parti con lo stesso nome.
Moduli del kit (in fondo al file): kit_sottomarini (scafo_sottomarino, vela, timone_verticale, pinna_sottomarino,
pinna_orizzontale, pod, solido_rivoluzione), kit_missili (corpo_missile, pinna_missile, pinne, gondola,
presa_ventrale, anello), kit_aerei (fusoliera_aereo, corpo_aereo, ala, elica, ellissoide_aereo), kit_elicotteri
(fusoliera_elicottero, piano, deriva_y, ellissoide_elicottero, ruota, tubo_inclinato, disco_rotore).
Materiale 'retinato' per eliche e rotori in movimento (vedi costruisci.py).
"""
import numpy as np

class Mesh:
    def __init__(self, tri=None, mat=None, parte=None):
        self.tri = np.zeros((0, 3, 3)) if tri is None else np.asarray(tri, float).reshape(-1, 3, 3)
        self.mat = list(mat or [])
        self.parte = list(parte or [])

    def __add__(self, o):
        return Mesh(np.concatenate([self.tri, o.tri]), self.mat + o.mat, self.parte + o.parte)

    def nome(self, n):
        self.parte = [n] * len(self.tri); return self

    def sposta(self, dx=0.0, dy=0.0, dz=0.0):
        return Mesh(self.tri + np.array([dx, dy, dz]), self.mat, self.parte)

    def ruota_y(self, gradi, cx=0.0, cz=0.0):
        """Rotazione attorno a un asse parallelo a y (beccheggio della parte), centro (cx, cz)."""
        a = np.radians(gradi); c, s = np.cos(a), np.sin(a)
        t = self.tri.copy(); x, z = t[..., 0] - cx, t[..., 2] - cz
        t[..., 0], t[..., 2] = cx + c * x - s * z, cz + s * x + c * z
        return Mesh(t, self.mat, self.parte)

    def ruota_z(self, gradi, cx=0.0, cy=0.0):
        a = np.radians(gradi); c, s = np.cos(a), np.sin(a)
        t = self.tri.copy(); x, y = t[..., 0] - cx, t[..., 1] - cy
        t[..., 0], t[..., 1] = cx + c * x - s * y, cy + s * x + c * y
        return Mesh(t, self.mat, self.parte)

def _quad(a, b, c, d):
    return [(a, b, c), (a, c, d)]

def _mesh(tris, mat):
    return Mesh(tris, [mat] * len(tris))

# ------------------------------------------------------------------ primitive
def scatola(x0, x1, y0, y1, z0, z1, mat='sovrastruttura', rastrema=1.0, smusso_prua=0.0):
    """Parallelepipedo da x0..x1, y0..y1, z0..z1. `rastrema` riduce la sezione in alto (attorno al
    centro); `smusso_prua` arretra la faccia superiore verso poppa (fronte inclinato, metri)."""
    cx, cy = (x0 + x1) / 2, (y0 + y1) / 2
    hx, hy = (x1 - x0) / 2, (y1 - y0) / 2
    tx0 = cx - hx * rastrema; tx1 = cx + hx * rastrema - smusso_prua
    ty0, ty1 = cy - hy * rastrema, cy + hy * rastrema
    v = [(x0, y0, z0), (x1, y0, z0), (x1, y1, z0), (x0, y1, z0),
         (tx0, ty0, z1), (tx1, ty0, z1), (tx1, ty1, z1), (tx0, ty1, z1)]
    f = [(4, 5, 6, 7), (0, 1, 5, 4), (1, 2, 6, 5), (2, 3, 7, 6), (3, 0, 4, 7), (0, 3, 2, 1)]
    tri = []
    for a, b, c, d in f: tri += _quad(v[a], v[b], v[c], v[d])
    return _mesh(tri, mat)

def asta(p0, p1, spessore=0.25, mat='traliccio'):
    """Asta sottile (sezione quadrata) tra due punti: elemento di tralicci, pennoni, ringhiere."""
    p0, p1 = np.asarray(p0, float), np.asarray(p1, float)
    d = p1 - p0; L = np.linalg.norm(d)
    if L < 1e-6: return Mesh()
    d /= L
    a = np.cross(d, [0, 0, 1.0]) if abs(d[2]) < 0.9 else np.cross(d, [1.0, 0, 0])
    a /= np.linalg.norm(a); b = np.cross(d, a)
    h = spessore / 2
    q = [a * h + b * h, -a * h + b * h, -a * h - b * h, a * h - b * h]
    tri = []
    for k in range(4):
        u, w = q[k], q[(k + 1) % 4]
        tri += _quad(p0 + u, p0 + w, p1 + w, p1 + u)
    return _mesh(tri, mat)

def cilindro(x, y, r, z0, z1, mat='sovrastruttura', n=14, r_alto=None):
    r1 = r if r_alto is None else r_alto
    tri = []
    for k in range(n):
        a0, a1 = 2 * np.pi * k / n, 2 * np.pi * (k + 1) / n
        b0 = (x + r * np.cos(a0), y + r * np.sin(a0), z0); b1 = (x + r * np.cos(a1), y + r * np.sin(a1), z0)
        t0 = (x + r1 * np.cos(a0), y + r1 * np.sin(a0), z1); t1 = (x + r1 * np.cos(a1), y + r1 * np.sin(a1), z1)
        tri += _quad(b0, b1, t1, t0) + [((x, y, z1), t0, t1)]
    return _mesh(tri, mat)

def sfera(x, y, z, r, mat='bianco', n=10, mezza=False):
    tri = []
    nt = n // 2 if mezza else n
    for i in range(nt):
        t0, t1 = np.pi * i / n, np.pi * (i + 1) / n
        for k in range(2 * n):
            a0, a1 = np.pi * k / n, np.pi * (k + 1) / n
            P = lambda t, a: (x + r * np.sin(t) * np.cos(a), y + r * np.sin(t) * np.sin(a), z + r * np.cos(t))
            tri += _quad(P(t0, a0), P(t1, a0), P(t1, a1), P(t0, a1))
    return _mesh(tri, mat)

# ------------------------------------------------------------------ scafo
def scafo(L, B, bordo, slancio=0.0, finezza_prua=40.0, specchio=0.82, svasatura=0.04, stazioni=80,
          bagnasciuga=1.0, pienezza=(0.8, 0.45), profilo_prua=None, prua_segue_dritto=False, mat_specchio='scafo',
          specchio_alto=None, lunghezza_poppa=30.0, poppa_tonda=None):
    """Generatore di scafi.
    L, B: lunghezza fuori tutto e larghezza massima (m).
    bordo: lista di punti (distanza dalla prua in m, quota della coperta in m), interpolata.
    slancio: arretramento del dritto di prua al galleggiamento (m).
    finezza_prua: lunghezza (m) dalla prua su cui lo scafo si allarga fino a B.
    specchio: larghezza relativa dello specchio di poppa. svasatura: allargamento relativo in coperta.
    bagnasciuga: altezza (m) della fascia scura sopra il galleggiamento.
    pienezza: esponenti della forma di prua al galleggiamento e in coperta (piu' basso = piu' pieno):
    con (0.8, 0.45) la prua e' fine al galleggiamento e svasata in coperta.
    profilo_prua: profilo del dritto tabulato [[d, z], ...] (distanza dalla prua, quota del bordo inferiore
    della murata in quel punto), al posto del dritto rettilineo dato da `slancio`: permette dritti curvi
    o concavi (es. [[0, 9], [3, 5], [7, 2], [11, 0]]).
    prua_segue_dritto: la larghezza delle sezioni cresce dalla distanza dal dritto alla stessa quota, non
    dalla prua in coperta: con il dritto slanciato il piede di prua resta fine invece che tozzo.
    mat_specchio: materiale dello specchio di poppa.
    specchio_alto: larghezza relativa dello specchio in coperta (predefinito = `specchio`): con
    specchio < specchio_alto lo specchio e' a V, stretto al galleggiamento e largo in coperta.
    lunghezza_poppa: tratto (m) su cui lo scafo si restringe verso lo specchio.
    poppa_tonda: {lunghezza: Lp, pendenza: a, soglia: b}: poppa che si stringe verso il galleggiamento fino
    all'asse (petroliere, mercantili): nei Lp metri poppieri la semilarghezza a quota z e' moltiplicata per
    g + (1 - g) * t / Lp, con t distanza dalla poppa e g = clip(a * z / quota della coperta - b, 0, 1).
    Per poppe a incrociatore con volta sopra il timone c'e' anche `scafo_composto`.
    """
    XB = L / 2
    bd = np.array(sorted(bordo)); dp, zp = bd[:, 0], bd[:, 1]
    def zc(x): return np.interp(XB - x, dp, zp)
    if profilo_prua:
        pp = np.array(sorted(profilo_prua), float); pd, pz = pp[:, 0], pp[:, 1]
        def zf(x):     # bordo inferiore della murata lungo il dritto tabulato
            return min(float(np.interp(XB - x, pd, pz, right=0.0)), zc(x) * 0.98)
        lung_dritto = float(pd.max())
    else:
        def zf(x):     # bordo inferiore della murata: sale lungo il dritto di prua slanciato
            if slancio <= 0: return 0.0
            return np.clip((x - (XB - slancio)) / slancio, 0, 1) * zc(x) * 0.98
        lung_dritto = float(slancio)
    if prua_segue_dritto and lung_dritto > 0:
        # distanza del dritto dalla prua in funzione della quota (inversa di zf, resa monotona)
        dg = np.linspace(0, lung_dritto, 200); zg = np.minimum.accumulate([zf(XB - d) for d in dg])
        def d_dritto(z): return float(np.interp(z, zg[::-1], dg[::-1]))
    else:
        def d_dritto(z): return 0.0
    sa = specchio if specchio_alto is None else specchio_alto
    def sl(x, z):
        e = pienezza[0] + (pienezza[1] - pienezza[0]) * np.clip(z / max(zp.max(), 1), 0, 1)
        t = np.clip((XB - d_dritto(z) - x) / finezza_prua, 0, 1) ** e
        sz = specchio if specchio_alto is None else specchio + (sa - specchio) * np.clip(z / max(zp.max(), 1), 0, 1)
        p = sz + (1 - sz) * np.clip((x + XB) / lunghezza_poppa, 0, 1)
        return B / 2 * t * p * (1 + svasatura * z / max(zp.max(), 1))
    xs = np.linspace(-XB, XB, stazioni)
    if profilo_prua:                  # stazioni anche nei punti del profilo, dove la forma cambia
        xs = np.unique(np.concatenate([xs, XB - pd[(pd > 0) & (pd < L)]]))
    tri, mat = [], []
    for i in range(len(xs) - 1):
        x0, x1 = xs[i], xs[i + 1]
        for s in (1, -1):
            for za, zb, m in ((0.0, bagnasciuga, 'bagnasciuga'), (bagnasciuga, None, 'scafo')):
                a0, a1 = max(za, zf(x0)), max(za, zf(x1))
                t0 = max(zb if zb is not None else zc(x0), a0); t1 = max(zb if zb is not None else zc(x1), a1)
                q = _quad((x0, s * sl(x0, a0), a0), (x1, s * sl(x1, a1), a1), (x1, s * sl(x1, t1), t1), (x0, s * sl(x0, t0), t0))
                tri += q; mat += [m, m]
        z0, z1 = zc(x0), zc(x1)
        tri += _quad((x0, -sl(x0, z0), z0), (x1, -sl(x1, z1), z1), (x1, sl(x1, z1), z1), (x0, sl(x0, z0), z0)); mat += ['ponte'] * 2
        f0, f1 = zf(x0), zf(x1)
        if f0 > 0 or f1 > 0:   # chiusura sotto il dritto slanciato (altrimenti resta una fessura nelle viste di prua)
            tri += _quad((x0, -sl(x0, f0), f0), (x0, sl(x0, f0), f0), (x1, sl(x1, f1), f1), (x1, -sl(x1, f1), f1)); mat += ['bagnasciuga'] * 2
    z0 = zc(-XB); w0, w1 = sl(-XB, 0), sl(-XB, z0)
    tri += _quad((-XB, -w0, 0), (-XB, w0, 0), (-XB, w1, z0), (-XB, -w1, z0)); mat += [mat_specchio] * 2
    m = Mesh(tri, mat).nome('scafo')
    if poppa_tonda:
        Lp = float(poppa_tonda.get('lunghezza', 40.0)); a = float(poppa_tonda.get('pendenza', 1.6))
        b = float(poppa_tonda.get('soglia', 0.15))
        T = m.tri.copy(); x, y, z = T[..., 0], T[..., 1], T[..., 2]
        tp = np.clip((x + XB) / Lp, 0, 1)
        zcx = np.vectorize(zc)(x)
        g = np.clip(a * z / np.maximum(zcx, 1e-6) - b, 0, 1)
        T[..., 1] = y * (g + (1 - g) * tp)
        m = Mesh(T, m.mat, m.parte)
    m.quota_coperta = zc; m.semilarghezza = sl
    return m

# ------------------------------------------------------------------ parti composte
def traliccio(x, base, cima, z0, z1, piani=(), gambe=4, mat='traliccio', spessore=0.3, y=0.0):
    """Albero a traliccio a sezione quadrata: `base` e `cima` sono i lati (m) in basso e in alto.
    Gambe d'angolo, controventi a X tra i piani e piattaforme ai `piani` (quote in m)."""
    def lato(z): return base + (cima - base) * (z - z0) / (z1 - z0)
    ang = [(1, 1), (1, -1), (-1, -1), (-1, 1)][:gambe]
    m = Mesh()
    for sx, sy in ang:
        m = m + asta((x + sx * base / 2, y + sy * base / 2, z0), (x + sx * cima / 2, y + sy * cima / 2, z1), spessore, mat)
    livelli = sorted(set([z0, z1] + list(piani)))
    for za, zb in zip(livelli[:-1], livelli[1:]):
        la, lb = lato(za) / 2, lato(zb) / 2
        for k in range(len(ang)):
            (ax, ay), (bx, by) = ang[k], ang[(k + 1) % len(ang)]
            m = m + asta((x + ax * la, y + ay * la, za), (x + bx * lb, y + by * lb, zb), spessore * 0.7, mat)
            m = m + asta((x + bx * la, y + by * la, za), (x + ax * lb, y + ay * lb, zb), spessore * 0.7, mat)
    for z in piani:
        l = lato(z) / 2
        m = m + scatola(x - l - 0.4, x + l + 0.4, y - l - 0.4, y + l + 0.4, z - 0.15, z + 0.15, mat)
    return m

def pennone(x, z, semilunghezza, y=0.0, spessore=0.25, mat='traliccio'):
    return asta((x, y - semilunghezza, z), (x, y + semilunghezza, z), spessore, mat)

def antenna_radar(x, z0, larghezza, altezza, y=0.0, orientamento=0.0, sbarre=6, mat='scuro', spessore=0.2):
    """Antenna a graticcio piana (es. SPS-49): cornice + sbarre orizzontali, ruotata di `orientamento`
    gradi attorno a z (0 = faccia rivolta verso prua)."""
    m = Mesh(); h = larghezza / 2
    for k in range(sbarre + 1):
        zz = z0 + altezza * k / sbarre
        m = m + asta((x, y - h, zz), (x, y + h, zz), spessore, mat)
    for yy in (-h, 0.0, h):
        m = m + asta((x, y + yy, z0), (x, y + yy, z0 + altezza), spessore, mat)
    return m.ruota_z(orientamento, x, y)

def lanciatore(x, z0, altezza=3.0, braccio=3.5, r=1.3, y=0.0):
    """Lanciatore a braccio singolo (es. Mk13): piedistallo cilindrico + braccio."""
    return (cilindro(x, y, r, z0, z0 + altezza * 0.6) +
            scatola(x - braccio / 2, x + braccio / 2, y - 0.35, y + 0.35, z0 + altezza * 0.6, z0 + altezza))

def cannone(x, z0, torretta=(3.0, 2.8, 1.3), canna=4.5, y=0.0, verso=1.0):
    """Torretta + canna rivolta verso prua (verso=+1) o poppa (-1)."""
    lx, ly, h = torretta
    t = scatola(x - lx / 2, x + lx / 2, y - ly / 2, y + ly / 2, z0, z0 + h, rastrema=0.85)
    c0 = x + verso * lx / 2
    return t + scatola(min(c0, c0 + verso * canna), max(c0, c0 + verso * canna), y - 0.15, y + 0.15,
                       z0 + h * 0.55, z0 + h * 0.75)

def ciws(x, z0, y=0.0):
    """Sistema di difesa ravvicinata tipo Phalanx: base, cupola bianca, canne."""
    return (cilindro(x, y, 0.9, z0, z0 + 0.8) + cilindro(x, y, 0.8, z0 + 0.8, z0 + 1.6, 'bianco') +
            sfera(x, y, z0 + 1.6, 0.8, 'bianco', mezza=True))

def radome(x, z, r, y=0.0, base=1.6):
    return scatola(x - base / 2, x + base / 2, y - base / 2, y + base / 2, z - r - 2.0, z - r * 0.6) + sfera(x, y, z, r)

def fumaiolo(x0, x1, semilarghezza, z0, z1, y=0.0, rastrema=0.92):
    return scatola(x0, x1, y - semilarghezza, y + semilarghezza, z0, z1, rastrema=rastrema)

def imbarcazione(x0, x1, y, z0, larghezza=2.2, altezza=1.2):
    """Lancia o battello in gruetta: scafetto affusolato semplificato."""
    return scatola(x0, x1, y - larghezza / 2, y + larghezza / 2, z0, z0 + altezza, 'bianco', rastrema=0.85)

# ------------------------------------------------------------------ parti sovietiche
def tubo(x0, x1, y, z, r, mat='sovrastruttura', n=10):
    """Cilindro orizzontale lungo x (tubi di lanciatori, canne grosse)."""
    tri = []
    for k in range(n):
        a0, a1 = 2 * np.pi * k / n, 2 * np.pi * (k + 1) / n
        p = lambda x, a: (x, y + r * np.cos(a), z + r * np.sin(a))
        tri += _quad(p(x0, a0), p(x1, a0), p(x1, a1), p(x0, a1))
        tri += [((x0, y, z), p(x0, a1), p(x0, a0)), ((x1, y, z), p(x1, a0), p(x1, a1))]
    return _mesh(tri, mat)

def lanciatore_quadruplo(x0, x1, z0, larghezza=4.6, altezza=3.6, r_tubo=0.75, y=0.0, sporgenza=1.2):
    """Lanciatore a quattro tubi in cassone (es. URPK-5 Rastrub, SS-N-14): basamento, cassone e quattro
    tubi 2x2 che sporgono verso prua con le estremita' a vista."""
    zc = z0 + altezza * 0.62; dy = larghezza / 4; dz = altezza * 0.2
    m = scatola(x0, x1, y - larghezza / 2, y + larghezza / 2, z0, z0 + altezza * 0.3, rastrema=0.95)
    m = m + scatola(x0, x1 - sporgenza, y - larghezza / 2, y + larghezza / 2, z0 + altezza * 0.3, z0 + altezza, rastrema=0.97)
    for sy in (-1, 1):
        for sz in (-1, 1):
            m = m + tubo(x1 - sporgenza - 1.0, x1, y + sy * dy, zc + sz * dz, r_tubo)
    return m

def lanciatore_scomparsa(x, z0, r=1.9, altezza=1.2, y=0.0):
    """Lanciatore missilistico a scomparsa (es. Osa-M / SA-N-4): pozzo cilindrico con cupola bassa."""
    return cilindro(x, y, r, z0, z0 + altezza, n=16) + sfera(x, y, z0 + altezza, r * 0.9, 'sovrastruttura', n=8, mezza=True).sposta(dz=-r * 0.45)

def lanciarazzi_rbu(x, z0, y=0.0, tubi=(3, 4), elevazione=40.0):
    """Lanciarazzi antisommergibile a canne multiple (es. RBU-6000): piedistallo e fascio di canne inclinato."""
    m = cilindro(x, y, 0.7, z0, z0 + 1.0)
    nr, nc = tubi; L = 1.6
    blocco = Mesh()
    for i in range(nr):
        for j in range(nc):
            blocco = blocco + tubo(-L / 2, L / 2, (j - (nc - 1) / 2) * 0.35, (i - (nr - 1) / 2) * 0.35, 0.15, 'scuro', n=6)
    return m + blocco.ruota_y(elevazione).sposta(x, y, z0 + 1.5)

def paraboloide(x, y, z, larghezza, altezza, profondita=0.6, orientamento=0.0, mat='scuro', n=8):
    """Antenna a riflettore parabolico (guscio sottile), concavita' verso +x prima della rotazione."""
    tri = []
    us = np.linspace(-0.5, 0.5, n + 1); vs = np.linspace(-0.5, 0.5, n // 2 + 1)
    P = lambda u, v: (x - profondita * (4 * u * u + 4 * v * v) / 2, y + u * larghezza, z + v * altezza)
    for i in range(n):
        for j in range(n // 2):
            tri += _quad(P(us[i], vs[j]), P(us[i + 1], vs[j]), P(us[i + 1], vs[j + 1]), P(us[i], vs[j + 1]))
    return _mesh(tri, mat).ruota_z(orientamento, x, y)

def head_net(x, z0, larghezza=6.0, altezza=1.6, y=0.0, orientamento=90.0):
    """Radar a due riflettori contrapposti (es. MR-310 Angara / Head Net-C): due paraboloidi schiena
    contro schiena, uno orizzontale e uno inclinato, sul supporto rotante."""
    m = cilindro(x, y, 0.4, z0, z0 + 0.8)
    a = paraboloide(x + 0.5, y, z0 + 1.6, larghezza, altezza, orientamento=0.0)
    b = paraboloide(x - 0.5, y, z0 + 1.6, larghezza, altezza, orientamento=180.0).ruota_y(-25, x, z0 + 1.6)
    return m + (a + b).ruota_z(orientamento, x, y)

def direttore_tiro(x, z0, altezza_base=2.5, y=0.0, larghezza=2.4, disco=1.4, orientamento=0.0):
    """Direttore di tiro su torretta (es. Pop Group, Owl Screech): base cilindrica, cabina e riflettore."""
    m = cilindro(x, y, 1.0, z0, z0 + altezza_base)
    m = m + scatola(x - 1.2, x + 1.2, y - larghezza / 2, y + larghezza / 2, z0 + altezza_base, z0 + altezza_base + 1.6, rastrema=0.9)
    m = m + paraboloide(x + 1.4, y, z0 + altezza_base + 1.0, disco, disco, profondita=0.4, mat='scuro', n=6)
    return m.ruota_z(orientamento, x, y)

def torre_singola(x, z0, lunghezza=5.5, larghezza=3.6, altezza=2.9, canna=5.0, y=0.0, verso=1.0):
    """Torre di cannone singola (es. AK-100): corpo con fronte arrotondato e tetto inclinato, canna."""
    r = larghezza / 2; xf = x + verso * (lunghezza / 2 - r)
    corpo = scatola(min(x - verso * lunghezza / 2, xf), max(x - verso * lunghezza / 2, xf), y - r, y + r, z0, z0 + altezza,
                    rastrema=0.85)
    fronte = cilindro(xf, y, r, z0, z0 + altezza * 0.85, n=12, r_alto=r * 0.85)
    c0 = x + verso * lunghezza / 2
    return corpo + fronte + tubo(min(c0, c0 + verso * canna), max(c0, c0 + verso * canna), y, z0 + altezza * 0.45, 0.18)

def lanciasiluri_quadruplo(x, y, z0, lunghezza=8.0, verso=1.0):
    """Lanciasiluri quadruplo da 533 mm sul ponte: affusto e quattro tubi affiancati."""
    m = scatola(x - 1.0, x + 1.0, y - 1.2, y + 1.2, z0, z0 + 0.9)
    for k in range(4):
        m = m + tubo(x - lunghezza / 2, x + lunghezza / 2, y - 0.9 + 0.6 * k, z0 + 1.3, 0.3)
    return m


# --- Scafo con poppa a incrociatore (dal mercantile, 23/10/2026) ---

def _meta_x(m, positiva):
    """Triangoli con il baricentro a x > 0 (positiva) o x < 0."""
    cx = m.tri[..., 0].mean(axis=1)
    k = np.flatnonzero(cx > 1e-6 if positiva else cx < -1e-6)
    return Mesh(m.tri[k], [m.mat[i] for i in k], [m.parte[i] for i in k] if m.parte else None)


def scafo_composto(L, B, bordo, profilo_prua, profilo_poppa, finezza_prua=45.0, finezza_poppa=30.0,
                   pienezza_prua=(0.8, 0.45), pienezza_poppa=(0.7, 0.3), svasatura=0.03, bagnasciuga=1.0,
                   stazioni=121, mat_scafo='scafo'):
    """Scafo con prua e poppa tabulate (poppa a incrociatore, con la volta sopra il timone): unione della meta'
    prodiera di uno scafo con `profilo_prua` e della meta' poppiera di un secondo scafo, con il profilo della
    poppa usato come dritto, riflesso in x. Si usa come generatore (`scafo: {generatore: scafo_composto, ...}`).
    bordo: [[d dalla prua, quota della coperta], ...]; profilo_prua: [[d dalla prua, quota del bordo inferiore
    della murata], ...]; profilo_poppa: [[d dalla POPPA, quota del bordo inferiore], ...] (0 al timone).
    Le due meta' si uniscono a meta' lunghezza (x = 0), dove le sezioni coincidono."""
    stazioni = int(stazioni) | 1          # numero dispari: x = 0 e' un confine tra stazioni
    comuni = dict(L=L, B=B, specchio=1.0, svasatura=svasatura, stazioni=stazioni, bagnasciuga=bagnasciuga,
                  prua_segue_dritto=True)
    pr = scafo(bordo=bordo, profilo_prua=profilo_prua, finezza_prua=finezza_prua, pienezza=pienezza_prua, **comuni)
    bordo_r = [[L - d, z] for d, z in bordo]
    pp = scafo(bordo=bordo_r, profilo_prua=profilo_poppa, finezza_prua=finezza_poppa, pienezza=pienezza_poppa, **comuni)
    m = _meta_x(pr, True) + _meta_x(_rifletti_x(pp, 0.0), False)
    if mat_scafo != 'scafo':
        m = Mesh(m.tri, [mat_scafo if x == 'scafo' else x for x in m.mat], m.parte)
    return m


# --- Parti NATO di cacciatorpediniere e incrociatori (dalla classe Kidd) ---

def lanciatore_mk26(x, z0, y=0.0, verso=1.0, elevazione=40.0, altezza_base=1.6, r_base=1.7,
                    braccio=4.9, missili=True, scarto_bracci=1.15):
    """Mk26: base cilindrica, giostra a cassone, due bracci paralleli (distanza 2*scarto_bracci) con
    missile (SM-2MR, 4,7 m) in posizione di lancio, puntati verso prua (verso=+1) o poppa (-1) con
    l'elevazione data (gradi)."""
    m = cilindro(x, y, r_base, z0, z0 + altezza_base, n=16)
    zc = z0 + altezza_base
    m = m + scatola(x - 1.4, x + 1.4, y - 1.6, y + 1.6, zc, zc + 1.5, rastrema=0.9)
    zp = zc + 1.2                                     # perno dei bracci
    br = Mesh()
    for s in (-1, 1):
        yy = y + s * scarto_bracci
        br = br + scatola(-0.6, braccio - 0.6, yy - 0.25, yy + 0.25, -0.35, 0.25, 'sovrastruttura')
        if missili:
            br = br + tubo(-0.2, 4.5, yy, -0.6, 0.18, 'bianco', n=8)
    br = br.ruota_y(-elevazione)                      # punta verso +x e in alto
    if verso < 0:
        br = br.ruota_z(180.0)
    return m + br.sposta(x, 0.0, zp)


def cannone_mk45(x, z0, y=0.0, verso=1.0, lunghezza=5.4, larghezza=3.2, altezza=2.5, canna=6.2):
    """Mk45 da 127/54: scudo basso con fronte arrotondato e tetto inclinato, canna lunga."""
    return torre_singola(x, z0, lunghezza=lunghezza, larghezza=larghezza, altezza=altezza, canna=canna,
                             y=y, verso=verso)


def sps48(x, z0, larghezza=5.2, altezza=5.6, spessore=0.7, y=0.0, inclinazione=18.0, orientamento=0.0,
          verso=1.0):
    """Antenna planare dell'SPS-48: pannello quasi quadrato inclinato all'indietro sul suo supporto.
    `verso`=+1 faccia verso prua, -1 verso poppa (il pannello si trova dal lato della faccia rispetto a x)."""
    p = scatola(-spessore / 2, spessore / 2, -larghezza / 2, larghezza / 2, 0.0, altezza, 'scuro')
    p = p.ruota_y(inclinazione)                       # cima spostata verso -x (indietro rispetto alla faccia)
    sup = scatola(-0.5, 0.5, -0.5, 0.5, -1.2, 0.0, 'traliccio')
    m = p + sup
    if verso < 0:
        m = m.ruota_z(180.0)
    return m.ruota_z(orientamento).sposta(x, y, z0)


def mack(x0, x1, y0, y1, z0, z1, cappello=1.6, scarichi=2, r_scarico=0.75, z_scarichi=None):
    """Fumaiolo tipo "mack" degli Spruance: tronco a pareti verticali con cappello nero (parte alta
    annerita dai fumi) e tubi di scarico neri che sporgono sopra."""
    m = scatola(x0, x1, y0, y1, z0, z1 - cappello, 'sovrastruttura', rastrema=0.98)
    m = m + scatola(x0, x1, y0, y1, z1 - cappello, z1, 'nero')
    zs = z1 + 0.9 if z_scarichi is None else z_scarichi
    cy = (y0 + y1) / 2
    for k in range(scarichi):
        xx = x0 + (x1 - x0) * (k + 0.5) / scarichi
        m = m + cilindro(xx, cy, r_scarico, z1, zs, 'nero', n=10)
    return m


def harpoon(x, y, z0, verso=1.0, elevazione=35.0, lunghezza=4.6):
    """Lanciatore quadruplo Mk141 per Harpoon: supporto e quattro contenitori inclinati affiancati,
    rivolti verso il fianco (verso=+1 verso sinistra, -1 verso dritta)."""
    m = scatola(x - 1.2, x + 1.2, y - 0.8, y + 0.8, z0, z0 + 1.0, 'traliccio')
    c = Mesh()
    for k in range(2):
        for j in range(2):
            yy = -0.4 + 0.8 * k
            c = c + scatola(-lunghezza / 2, lunghezza / 2, yy - 0.37, yy + 0.37, 0.75 * j, 0.75 * j + 0.7,
                            'sovrastruttura')
    c = c.ruota_y(-elevazione).ruota_z(90.0 * verso)
    return m + c.sposta(x, y, z0 + 1.2)


def albero_palo(x, z0, z1, r=0.55, piattaforme=(), crocette=(), y=0.0, mat='scuro'):
    """Albero a palo (tubo verticale) con piattaforme [quota, lato] e crocette [quota, semilunghezza]."""
    m = cilindro(x, y, r, z0, z1, mat, n=8, r_alto=r * 0.5)
    for z, l in piattaforme:
        m = m + scatola(x - l / 2, x + l / 2, y - l / 2, y + l / 2, z - 0.15, z + 0.15, 'traliccio')
    for z, s in crocette:
        m = m + asta((x, y - s, z), (x, y + s, z), 0.25, 'traliccio')
    return m

# --- Parti delle navi da battaglia (dalla classe Iowa) ---

def _rifletti_x(m, xc):
    t = m.tri.copy(); t[..., 0] = 2 * xc - t[..., 0]
    return Mesh(t, m.mat, m.parte)


def prisma_xz(profilo, y0, y1, mat='sovrastruttura'):
    """Profilo convesso [(x, z), ...] (in ordine) estruso tra y0 e y1: facce laterali e due tappi a ventaglio."""
    p = [tuple(map(float, q)) for q in profilo]; n = len(p); tri = []
    for i in range(n):
        (xa, za), (xb, zb) = p[i], p[(i + 1) % n]
        tri += [((xa, y0, za), (xb, y0, zb), (xb, y1, zb)), ((xa, y0, za), (xb, y1, zb), (xa, y1, za))]
    for k in range(1, n - 1):
        for y in (y0, y1):
            tri.append(((p[0][0], y, p[0][1]), (p[k][0], y, p[k][1]), (p[k + 1][0], y, p[k + 1][1])))
    return Mesh(tri, [mat] * len(tri))


def torre_trinata(x, z0, verso=1.0, z_ponte=None, lunghezza=18.0, larghezza=13.0, altezza=3.3, smusso=1.6,
                  r_barbetta=6.0, canna=14.5, z_canne=1.3, interasse=2.6, r_canna=0.42, y=0.0):
    """Torre trinata da 16"/50. x = centro della casamatta, z0 = base della casamatta (cielo della barbetta),
    z_ponte = quota del ponte su cui poggia la barbetta (se None, nessuna barbetta). `smusso`: arretramento
    del bordo superiore della faccia anteriore inclinata. Pianta della casamatta leggermente rastremata
    verso la faccia (come le torri Mk 7)."""
    h = lunghezza / 2; w = larghezza / 2
    m = Mesh()
    if z_ponte is not None and z0 > z_ponte:
        m = m + cilindro(x, y, r_barbetta, z_ponte, z0, n=20)
    # casamatta: profilo laterale (fronte inclinato) estruso; i fianchi rastremati con due prismi
    prof = [(x - h, z0), (x + h, z0), (x + h - smusso, z0 + altezza), (x - h, z0 + altezza)]
    m = m + prisma_xz(prof, y - w * 0.88, y + w * 0.88)
    # guance laterali (pianta piu' larga a poppa della torre)
    profg = [(x - h, z0), (x + h * 0.35, z0), (x + h * 0.35, z0 + altezza * 0.97), (x - h, z0 + altezza * 0.97)]
    m = m + prisma_xz(profg, y - w, y + w)
    # canne: dalla faccia verso prua
    xf = x + h - smusso * (z_canne / altezza)
    for k in (-1, 0, 1):
        m = m + tubo(xf - 1.0, xf + canna, y + k * interasse, z0 + z_canne, r_canna, n=8)
        m = m + tubo(xf - 0.6, xf + 1.2, y + k * interasse, z0 + z_canne, r_canna * 1.9, mat='sovrastruttura', n=8)
    if verso < 0: m = _rifletti_x(m, x)
    return m


def impianto_binato(x, y, z0, verso=1.0, lunghezza=5.4, larghezza=4.2, altezza=2.7, canna=4.6, smusso=1.4):
    """Impianto binato da 5"/38 Mk 28 (casamatta chiusa, fronte smussato) con due canne."""
    h = lunghezza / 2; w = larghezza / 2
    prof = [(x - h, z0), (x + h, z0), (x + h, z0 + altezza * 0.5), (x + h - smusso, z0 + altezza), (x - h, z0 + altezza)]
    m = prisma_xz(prof, y - w, y + w)
    for k in (-1, 1):
        m = m + tubo(x + h - 0.3, x + h + canna, y + k * 0.65, z0 + altezza * 0.55, 0.13, n=6)
    if verso < 0: m = _rifletti_x(m, x)
    return m


def abl(x, y, z0, lunghezza=6.4, larghezza=2.5, altezza=2.1, verso=1.0):
    """Armored Box Launcher Mk 143 (4 Tomahawk) in posizione di riposo: cassone su basamento."""
    h = lunghezza / 2; w = larghezza / 2
    m = scatola(x - h * 0.7, x + h * 0.7, y - w * 0.9, y + w * 0.9, z0, z0 + 0.6)
    prof = [(x - h, z0 + 0.6), (x + h, z0 + 0.6), (x + h, z0 + altezza - 0.3), (x + h - 0.4, z0 + altezza),
            (x - h, z0 + altezza)]
    m = m + prisma_xz(prof, y - w, y + w)
    if verso < 0: m = _rifletti_x(m, x)
    return m


def fumaiolo_cappello(x0, x1, semilarghezza, z0, z1, cappello=2.2, rastrema=0.95, y=0.0):
    """Fumaiolo con cappello scuro inclinato (piu' alto a prua), come sulle Iowa."""
    m = scatola(x0, x1, y - semilarghezza, y + semilarghezza, z0, z1, rastrema=rastrema)
    hx = (x1 - x0) / 2 * rastrema; cx = (x0 + x1) / 2; w = semilarghezza * rastrema
    prof = [(cx - hx, z1), (cx + hx, z1), (cx + hx * 0.6, z1 + cappello), (cx - hx * 0.3, z1 + cappello * 0.55)]
    return m + prisma_xz(prof, y - w * 0.95, y + w * 0.95, mat='nero')


def direttore_mk38(x, z0, larghezza_telemetro=10.0, y=0.0, altezza=2.4, lunghezza=4.2, verso=1.0):
    """Direttore Mk 38: casa con fronte arrotondato, telemetro trasversale a 'braccia' e cappuccio radar."""
    h = lunghezza / 2
    m = scatola(x - h, x + h, y - 1.9, y + 1.9, z0, z0 + altezza, rastrema=0.92)
    m = m + scatola(x - 0.6, x + 0.6, y - larghezza_telemetro / 2, y + larghezza_telemetro / 2,
                    z0 + altezza * 0.45, z0 + altezza * 0.85)
    m = m + scatola(x - 1.4, x + 0.8, y - 1.6, y + 1.6, z0 + altezza, z0 + altezza + 0.9, mat='scuro', rastrema=0.8)
    if verso < 0: m = _rifletti_x(m, x)
    return m


def direttore_mk37(x, y, z0, altezza_base=1.5, verso=1.0):
    """Direttore Mk 37: base cilindrica, casa e antenna Mk 25 (disco) sul tetto."""
    m = cilindro(x, y, 1.6, z0, z0 + altezza_base, n=12)
    m = m + scatola(x - 2.0, x + 2.0, y - 1.7, y + 1.7, z0 + altezza_base, z0 + altezza_base + 1.9, rastrema=0.9)
    m = m + paraboloide(x + 0.3, y, z0 + altezza_base + 2.9, 2.2, 2.2, profondita=0.4, n=6)
    if verso < 0: m = _rifletti_x(m, x)
    return m

# --- Parti delle portaerei (dalla classe Nimitz) ---

def sponsone(x0, x1, y0, y1, z, spessore=1.5, mensola=4.0, lato=None, mat='sovrastruttura'):
    """Piattaforma a sbalzo (sommita' a quota z) con mensola inclinata sotto, verso lo scafo.
    `lato`: +1 sponsone a sinistra (mensola che rientra verso y minori), -1 a dritta; None = solo lastra."""
    ya, yb = min(y0, y1), max(y0, y1)
    m = scatola(x0, x1, ya, yb, z - spessore, z, mat)
    if lato is not None and mensola > 0:
        zb = z - spessore
        yi = ya if lato > 0 else yb        # bordo interno (verso lo scafo)
        ye = yb if lato > 0 else ya        # bordo esterno
        tri = []
        tri += _quad((x0, yi, zb - mensola), (x1, yi, zb - mensola), (x1, ye, zb), (x0, ye, zb))
        if lato < 0: tri = [(a, c, b) for a, b, c in tri]
        tri += [((x0, yi, zb - mensola), (x0, ye, zb), (x0, yi, zb)), ((x1, yi, zb - mensola), (x1, yi, zb), (x1, ye, zb))]
        m = m + Mesh(tri, [mat] * len(tri))
    return m


def sea_sparrow(x, y, z0, orientamento=0.0, mat='sovrastruttura'):
    """Lanciatore Mk29 (NATO Sea Sparrow), otto celle 2x4: piedistallo, affusto a forcella, cassone
    di circa 4,2 x 2,6 x 2,1 m, elevato di 15 gradi; `orientamento` in gradi (0 = verso prua)."""
    m = cilindro(x, y, 1.1, z0, z0 + 1.6, mat, n=12)
    m = m + scatola(x - 0.8, x + 0.8, y - 1.6, y + 1.6, z0 + 1.6, z0 + 2.4, mat)
    cassa = scatola(-2.1, 2.1, -1.3, 1.3, -1.05, 1.05, mat).ruota_y(15).sposta(x + 0.3, y, z0 + 3.3)
    return (m + cassa).ruota_z(orientamento, x, y)


def aereo(x, y, z0, orientamento=0.0, lunghezza=17.0, apertura=9.0, deriva=4.8, mat='sovrastruttura'):
    """Sagoma bassa di un velivolo imbarcato parcheggiato (ali ripiegate): fusoliera rastremata,
    ali, piani di coda e deriva. Pochi triangoli (36): conta la sagoma, non il dettaglio.
    `orientamento`: direzione del muso in gradi (0 = verso prua, 90 = verso sinistra)."""
    L = lunghezza
    m = scatola(-L / 2, L / 2, -0.9, 0.9, 0.7, 2.6, mat, rastrema=0.85)                  # fusoliera
    m = m + scatola(-L * 0.20, L * 0.12, -apertura / 2, apertura / 2, 1.5, 1.8, mat)     # ali ripiegate
    m = m + scatola(-L * 0.5, -L * 0.34, -0.15, 0.15, 2.6, deriva, mat, smusso_prua=1.5)  # deriva
    return m.ruota_z(orientamento).sposta(x, y, z0)


def aerei(posizioni, z0, lunghezza=17.0, apertura=9.0, deriva=4.8, mat='sovrastruttura'):
    """Gruppo di velivoli parcheggiati: `posizioni` = [[x, y, orientamento], ...] o con un quarto
    valore = lunghezza propria."""
    m = Mesh()
    for p in posizioni:
        L = p[3] if len(p) > 3 else lunghezza
        m = m + aereo(p[0], p[1], z0, p[2], L, apertura * L / lunghezza, deriva, mat)
    return m


def antenna_piana(x, y, z0, larghezza=5.2, altezza=5.5, inclinazione=15.0, orientamento=0.0, sostegno=1.5, mat='scuro'):
    """Antenna radar piana a schiera (es. AN/SPS-48C): pannello inclinato all'indietro su un sostegno."""
    m = scatola(x - 0.7, x + 0.7, y - 0.7, y + 0.7, z0, z0 + sostegno, 'sovrastruttura')
    pan = scatola(-0.25, 0.25, -larghezza / 2, larghezza / 2, 0, altezza, mat).ruota_y(inclinazione)
    m = m + pan.sposta(x, y, z0 + sostegno)
    return m.ruota_z(orientamento, x, y)

# --- Parti delle navi ausiliarie e correzioni dello scafo (dalla classe Sacramento) ---

def chiusura_dritto(stazioni=12, **sc):
    """Chiude il dritto di prua slanciato dello scafo del kit. In kit.scafo il bordo inferiore delle murate
    sale lungo il dritto (slancio), ma su uno scafo largo come questo le due murate non si toccano: lungo quel
    bordo la semilarghezza resta di qualche metro e da prua si vede l'interno dello scafo (triangolo scuro
    sotto il dritto). Questa parte aggiunge la superficie inclinata che unisce i due bordi inferiori.
    Riceve gli stessi parametri dello scafo (L, B, bordo, slancio, ...)."""
    m = scafo(**sc)
    L, sl_ = sc['L'], sc.get('slancio', 0.0)
    if sl_ <= 0: return Mesh()
    XB = L / 2; zc, sl = m.quota_coperta, m.semilarghezza
    zf = lambda x: np.clip((x - (XB - sl_)) / sl_, 0, 1) * zc(x) * 0.98
    xs = np.linspace(XB - sl_, XB, stazioni + 1)
    tri, mat = [], []
    for x0, x1 in zip(xs[:-1], xs[1:]):
        a0, a1 = zf(x0), zf(x1)
        w0, w1 = sl(x0, a0), sl(x1, a1)
        q = [((x0, -w0, a0), (x1, -w1, a1), (x1, w1, a1)), ((x0, -w0, a0), (x1, w1, a1), (x0, w0, a0))]
        tri += q; mat += ['bagnasciuga' if max(a0, a1) < sc.get('bagnasciuga', 1.0) else 'scafo'] * 2
    return Mesh(tri, mat)


def stazioni_rifornimento(kingpost, bracci, z0, lato_kingpost=1.3, y_bracci=(-2.5, 2.5), spessore_braccio=0.8,
                          mat='traliccio', mat_kingpost='traliccio'):
    """Fila di stazioni di rifornimento lungo la coperta.
    kingpost: lista [x, z_cima] (colonne a sezione quadrata dal ponte z0 alla cima, sull'asse).
    bracci: lista di spezzate [[x, z], [x, z], ...] nel piano verticale longitudinale: i bracci inclinati
    (travi a traliccio) che uniscono le teste dei kingpost e i vertici dei telai ad A; ogni spezzata e'
    ripetuta ai valori di `y_bracci` (una trave per lato) con traverse tra le due travi ai vertici."""
    m = Mesh(); h = lato_kingpost / 2
    for x, zc in kingpost:
        m = m + scatola(x - h, x + h, -h, h, z0, zc, mat_kingpost)
    for linea in bracci:
        P = np.asarray(linea, float)
        for y in y_bracci:
            for (xa, za), (xb, zb) in zip(P[:-1], P[1:]):
                m = m + asta((xa, y, za), (xb, y, zb), spessore_braccio, mat)
        if len(y_bracci) > 1:
            for xv, zv in P:
                m = m + asta((xv, min(y_bracci), zv), (xv, max(y_bracci), zv), spessore_braccio * 0.8, mat)
    return m


def fumaiolo_cilindrico(x, z0, z1, r, cappello=3.8, allungamento=1.0, y=0.0, rastrema=0.95, n=18):
    """Fumaiolo a sezione circolare (o ellittica, allungata lungo x) con cappello scuro in cima."""
    zc = z1 - cappello
    corpo = cilindro(0, 0, r, z0, zc, 'sovrastruttura', n=n, r_alto=r * rastrema)
    capp = cilindro(0, 0, r * rastrema, zc, z1, 'nero', n=n, r_alto=r * rastrema * 0.97)
    m = corpo + capp
    t = m.tri.copy(); t[..., 0] *= allungamento
    return Mesh(t, m.mat, m.parte).sposta(x, y, 0)


def direttore_mk91(x, z0, altezza=2.4, y=0.0, r=1.0):
    """Direttore di tiro Mk 91 (radar Mk 95) su colonna: colonna, tamburo e antenna scura frontale."""
    m = cilindro(x, y, 0.45, z0, z0 + altezza)
    m = m + cilindro(x, y, r, z0 + altezza, z0 + altezza + 1.4, n=12)
    return m + scatola(x - 0.2, x + 0.2, y - r * 1.1, y + r * 1.1, z0 + altezza + 0.2, z0 + altezza + 1.3, 'nero')


def gru(base, cima, z0, colonna=1.2, spessore=0.7):
    """Gru da carico: colonna sul ponte e braccio inclinato dalla colonna alla testa (punti [x, y, z])."""
    bx, by, bz = base
    m = scatola(bx - colonna / 2, bx + colonna / 2, by - colonna / 2, by + colonna / 2, z0, bz, 'sovrastruttura')
    return m + asta(base, cima, spessore, 'traliccio')



# --- Parti sovietiche dal lotto del 23/10/2026 (Kara, Koni, Slava) ---

def tronco(basso, alto, z0, z1, y=0.0, mat='sovrastruttura'):
    """Tronco di piramide: `basso` e `alto` = [x0, x1, semilarghezza] delle due sezioni orizzontali
    (a quota z0 e z1). Permette fronte inclinato e retro verticale (torre del Kara)."""
    (a0, a1, wa), (b0, b1, wb) = basso, alto
    v = [(a0, y - wa, z0), (a1, y - wa, z0), (a1, y + wa, z0), (a0, y + wa, z0),
         (b0, y - wb, z1), (b1, y - wb, z1), (b1, y + wb, z1), (b0, y + wb, z1)]
    f = [(4, 5, 6, 7), (0, 1, 5, 4), (1, 2, 6, 5), (2, 3, 7, 6), (3, 0, 4, 7), (0, 3, 2, 1)]
    tri = []
    for a, b, c, d in f: tri += _quad(v[a], v[b], v[c], v[d])
    return Mesh(tri, [mat] * len(tri))


def ak630(x, y, z0, verso=1.0):
    """AK-630: basamento, torretta tonda bassa con cupola e canna corta."""
    m = cilindro(x, y, 1.0, z0, z0 + 0.6, n=10)
    m = m + cilindro(x, y, 0.95, z0 + 0.6, z0 + 1.5, n=10, r_alto=0.7)
    m = m + tubo(x, x + verso * 2.2, y, z0 + 1.1, 0.13, n=6)
    return m


def ak230(x, y, z0, r=1.05, altezza=1.15, canna=2.0, verso=1.0, orientamento=0.0):
    """AK-230: basamento (0,2 m), corpo cilindrico, cupola schiacciata e due canne verso prua (verso=+1)
    o verso poppa (-1), eventualmente ruotate di `orientamento` gradi attorno all'asse verticale."""
    m = cilindro(x, y, r * 1.08, z0, z0 + 0.2, n=12)
    m = m + cilindro(x, y, r, z0 + 0.2, z0 + altezza * 0.6, n=12)
    cupola = sfera(0, 0, 0, r, 'sovrastruttura', n=8, mezza=True)
    t = cupola.tri.copy(); t[..., 2] *= (altezza * 0.4) / r          # cupola schiacciata
    m = m + Mesh(t, cupola.mat).sposta(x, y, z0 + altezza * 0.6)
    zc = z0 + altezza * 0.55
    for k in (-1, 1):
        a, b = (x + r * 0.7, x + r * 0.7 + canna) if verso > 0 else (x - r * 0.7 - canna, x - r * 0.7)
        m = m + tubo(a, b, y + k * 0.22, zc, 0.07, 'scuro', n=6)
    return m.ruota_z(orientamento, x, y)


def _scala_z(m, cz, k):
    t = m.tri.copy(); t[..., 2] = cz + (t[..., 2] - cz) * k
    return Mesh(t, m.mat, m.parte)


def ak130(x, z0, y=0.0, verso=1.0, r=2.7, altezza=3.4, canna=6.5, interasse=1.1, z_canne=None, basamento=0.8):
    """Impianto binato AK-130 da 130 mm: basamento cilindrico basso, torre a pianta circolare con cielo
    bombato (calotta schiacciata) e due canne lunghe. `verso` +1 = canne verso prua."""
    m = cilindro(x, y, r * 1.05, z0, z0 + basamento, n=16)
    zt = z0 + basamento
    corpo = cilindro(x, y, r, zt, zt + altezza * 0.55, n=16, r_alto=r * 0.97)
    cupola = _scala_z(sfera(x, y, zt + altezza * 0.55, r * 0.97, 'sovrastruttura', n=12, mezza=True),
                      zt + altezza * 0.55, (altezza * 0.45) / (r * 0.97))
    m = m + corpo + cupola
    zc = zt + altezza * 0.42 if z_canne is None else z_canne
    x0 = x + verso * r * 0.8
    for s in (-1, 1):
        m = m + tubo(min(x0, x0 + verso * canna), max(x0, x0 + verso * canna), y + s * interasse / 2, zc, 0.22, n=8)
    return m


PARTI = dict(scatola=scatola, tuga=scatola, asta=asta, cilindro=cilindro, sfera=sfera, traliccio=traliccio,
             pennone=pennone, antenna_radar=antenna_radar, lanciatore=lanciatore, cannone=cannone, ciws=ciws,
             radome=radome, fumaiolo=fumaiolo, imbarcazione=imbarcazione, tubo=tubo,
             lanciatore_quadruplo=lanciatore_quadruplo, lanciatore_scomparsa=lanciatore_scomparsa,
             lanciarazzi_rbu=lanciarazzi_rbu, paraboloide=paraboloide, head_net=head_net,
             direttore_tiro=direttore_tiro, torre_singola=torre_singola, lanciasiluri_quadruplo=lanciasiluri_quadruplo,
             lanciatore_mk26=lanciatore_mk26, cannone_mk45=cannone_mk45, sps48=sps48, mack=mack,
             harpoon=harpoon, albero_palo=albero_palo, prisma_xz=prisma_xz, torre_trinata=torre_trinata,
             impianto_binato=impianto_binato, abl=abl, fumaiolo_cappello=fumaiolo_cappello, direttore_mk38=direttore_mk38,
             direttore_mk37=direttore_mk37, sponsone=sponsone, sea_sparrow=sea_sparrow, aereo=aereo,
             aerei=aerei, antenna_piana=antenna_piana, chiusura_dritto=chiusura_dritto, stazioni_rifornimento=stazioni_rifornimento,
             fumaiolo_cilindrico=fumaiolo_cilindrico, direttore_mk91=direttore_mk91, gru=gru,
             tronco=tronco, ak630=ak630, ak230=ak230, ak130=ak130, scafo_composto=scafo_composto)


# --- Moduli del kit per sottomarini, missili, aerei ed elicotteri (23/10/2026) ---
# Nati come moduli di classe dei lotti in parallelo; i nomi in conflitto ricevono un suffisso.
import kit_sottomarini, kit_missili, kit_aerei, kit_elicotteri  # noqa: E402

def _registra(mod, rinomina):
    for nome, f in mod.PARTI.items():
        PARTI[rinomina.get(nome, nome)] = f

_registra(kit_sottomarini, {'pinna': 'pinna_sottomarino'})
_registra(kit_missili, {'pinna': 'pinna_missile'})
_registra(kit_aerei, {'fusoliera': 'fusoliera_aereo', 'ellissoide': 'ellissoide_aereo', 'corpo': 'corpo_aereo'})
_registra(kit_elicotteri, {'fusoliera': 'fusoliera_elicottero', 'ellissoide': 'ellissoide_elicottero'})
