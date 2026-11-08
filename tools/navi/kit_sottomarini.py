"""Modulo del kit: parti per sottomarini in superficie (dal lotto del 23/10/2026: 688, Victor III, Kilo).
(Nato come tools/navi/classi/sottomarini_parti.py; registrato in kit.PARTI.)
"""
"""Parti dei sottomarini in emersione (modulo `moduli_parti` delle classi Los Angeles, Victor III, Kilo e
dei lotti successivi: Alfa, Akula, Oscar II, Typhoon, Charlie II, Foxtrot, Sturgeon, Trafalgar).

Coordinate del kit (tools/navi/kit.py): x verso prua (origine a meta' lunghezza), y verso sinistra, z verso
l'alto, galleggiamento a z = 0. Tutte le quote sono assolute rispetto al galleggiamento dell'emersione di
riferimento (quella delle foto). Ogni parte accetta `dz` (metri, predefinito 0), che sposta in verticale la
parte: con lo stesso valore su scafo e parti (in YAML un'ancora, `dz: &dz -0.8` e poi `dz: *dz`) si ottiene
l'assetto "a fior d'acqua" (dz < 0) o un'emersione piu' alta (dz > 0) senza riscrivere le quote.

Le parti vengono tagliate esattamente al galleggiamento (`taglia_galleggiamento`): sotto z = 0 non resta
geometria (il renderer altrimenti schiaccerebbe i vertici sommersi su z = 0, allargando la linea d'acqua).

Parti:
  scafo_sottomarino  generatore dello scafo (`scafo: {generatore: scafo_sottomarino, ...}`): solido di
                     rivoluzione a raggio tabulato (o forma a goccia parametrica), sezione ellittica o
                     superellittica, asse sotto il galleggiamento, cassero piatto tabulato (coperta);
  vela               falsatorre a livelli orizzontali (pianta a profilo alare o ovale), con timoni di vela
                     e alberi/periscopi alzati;
  timone_verticale   timone superiore (pinna) con il "pod" facoltativo in cima (Victor III: array rimorchiato)
                     e timoni orizzontali facoltativi;
  pinna              pinna/deriva generica a profilo sottile (timoni, alette, piani prodieri);
  solido_rivoluzione corpo di rivoluzione ad asse orizzontale (pod, carenature, cupole, cilindri di lancio).
"""
import os, sys
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import kit
from kit import Mesh

# ------------------------------------------------------------------ utilita'
def taglia_galleggiamento(m, z0=0.0, eps=1e-6):
    """Taglio esatto dei triangoli al piano z = z0: resta solo la parte emersa (1 o 2 triangoli per
    triangolo attraversato dal piano). Materiali e nomi delle parti si conservano."""
    T = m.tri
    if len(T) == 0: return m
    d = T[..., 2] - z0
    sopra = d >= -eps
    ns = sopra.sum(1)
    tri, mat, par = [], [], []
    tutti = ns == 3
    tri.append(T[tutti]); mat += [m.mat[i] for i in np.flatnonzero(tutti)]; par += [m.parte[i] for i in np.flatnonzero(tutti)] if m.parte else []
    for i in np.flatnonzero((ns == 1) | (ns == 2)):
        t, s, dd = T[i], sopra[i], d[i]
        # ruota i vertici in modo che il "solitario" (unico sopra o unico sotto) sia il primo
        k = int(np.flatnonzero(s if ns[i] == 1 else ~s)[0])
        a, b, c = t[k], t[(k + 1) % 3], t[(k + 2) % 3]
        da, db, dc = dd[k], dd[(k + 1) % 3], dd[(k + 2) % 3]
        ab = a + (b - a) * (da / (da - db)); ac = a + (c - a) * (da / (da - dc))
        ab[2] = ac[2] = z0
        nuovi = [(a, ab, ac)] if ns[i] == 1 else [(ab, b, c), (ab, c, ac)]
        for q in nuovi:
            tri.append(np.array(q)[None]); mat.append(m.mat[i])
            if m.parte: par.append(m.parte[i])
    tri = np.concatenate(tri) if tri else np.zeros((0, 3, 3))
    # via i triangoli degeneri (area nulla: punte dei solidi di rivoluzione)
    area = np.linalg.norm(np.cross(tri[:, 1] - tri[:, 0], tri[:, 2] - tri[:, 0]), axis=1)
    ok = area > 1e-9
    return Mesh(tri[ok], [x for x, o in zip(mat, ok) if o], [x for x, o in zip(par, ok) if o] if par else None)

def _anelli(P, chiuso=True):
    """Triangoli tra anelli consecutivi di punti P (stazioni, punti per anello, 3)."""
    ns, nk = P.shape[:2]
    tri = []
    kk = range(nk) if chiuso else range(nk - 1)
    for i in range(ns - 1):
        for k in kk:
            k1 = (k + 1) % nk
            tri += kit._quad(P[i, k], P[i + 1, k], P[i + 1, k1], P[i, k1])
    return tri

def _stazioni(L, n, addensa=0.5):
    """Distanze dalla prua 0..L, addensate alle estremita' (prua e poppa arrotondate) di un fattore 0..1."""
    t = np.linspace(0, 1, n)
    return L * ((1 - addensa) * t + addensa * (1 - np.cos(np.pi * t)) / 2)

def _tab(tab, d, fuori=np.nan):
    """Interpolazione lineare di una tabella [[d, v], ...] (d crescente) nei punti d; `fuori` fuori range."""
    a = np.asarray(tab, float)
    v = np.interp(d, a[:, 0], a[:, 1])
    return np.where((d < a[0, 0] - 1e-9) | (d > a[-1, 0] + 1e-9), fuori, v)

def forma_goccia(L, R, prua, poppa, esponente_prua=2.0, esponente_poppa=2.0, raggio_coda=0.0):
    """Raggio tabulato di uno scafo "a goccia": ogiva ellittica (superellittica) di lunghezza `prua`, tratto
    cilindrico, cono di coda di lunghezza `poppa` (esponente 1 = cono, 2 = coda arrotondata), raggio finale
    `raggio_coda` (mozzo dell'elica). Restituisce [[d, r], ...] per `raggio`."""
    d = _stazioni(L, 81, 0.6)
    r = np.full_like(d, R)
    m = d < prua
    r[m] = R * (1 - (1 - d[m] / prua) ** esponente_prua) ** (1 / esponente_prua)
    m = d > L - poppa
    t = (d[m] - (L - poppa)) / poppa
    r[m] = raggio_coda + (R - raggio_coda) * (1 - t ** esponente_poppa)
    return [[float(a), float(b)] for a, b in zip(d, r)]

# ------------------------------------------------------------------ scafo
def scafo_sottomarino(L, B, raggio=None, forma=None, bordo_libero=None, asse=None, ellitticita=1.0,
                      esponente_sezione=2.0, cassero=None, spigolo_cassero=6.0, assetto=0.0, stazioni=90,
                      punti_sezione=48, addensa=0.5, dz=0.0, mat='scafo', taglia=True, **_):
    """Scafo resistente/esterno di un sottomarino come solido di rivoluzione, tagliato al galleggiamento.

    L, B: lunghezza fuori tutto e diametro massimo (m).
    raggio: [[d dalla prua, r], ...] raggio dello scafo lungo la lunghezza (0 alle estremita' per prua e
            poppa chiuse); in alternativa `forma: {prua, poppa, esponente_prua, esponente_poppa, raggio_coda}`
            (vedi `forma_goccia`), con R = B/2.
    asse: quota dell'asse (negativa: sotto il galleggiamento), numero o [[d, z], ...]; se manca si ricava da
          `bordo_libero`, la quota della sommita' dello scafo (senza cassero) nella sezione di diametro
          massimo. Valori tipici: 0,8-1,5 m in emersione normale, 0,1-0,4 "a fior d'acqua".
    assetto: gradi, positivo = prua alta (l'asse ruota attorno a meta' lunghezza).
    ellitticita: semiasse verticale / semiasse orizzontale della sezione; esponente_sezione 2 = ellisse,
          > 2 = sezione piu' squadrata (scafi esterni dei sovietici).
    cassero: [[d, quota, semilarghezza], ...] coperta piatta del cassero: sopra `quota` lo scafo e' tagliato
          (coperta piana), sotto il cassero si unisce allo scafo con fianchi quasi verticali larghi
          `semilarghezza` (spigoli arrotondati da `spigolo_cassero`, esponente di superellisse). Fuori dal
          tratto tabulato non c'e' cassero.
    stazioni, punti_sezione, addensa: risoluzione (stazioni addensate a prua e a poppa).
    Parametri ignorati (**_) per compatibilita' con la sezione `scafo` del YAML (es. `bordo`)."""
    if raggio is None:
        f = dict(prua=0.15 * L, poppa=0.3 * L); f.update(forma or {})
        raggio = forma_goccia(L, B / 2, **f)
    d = np.unique(np.r_[_stazioni(L, int(stazioni), addensa), np.asarray(raggio, float)[:, 0]])
    if cassero:
        dc = np.asarray(cassero, float)[:, 0]
        d = np.unique(np.r_[d, dc, np.clip(dc - 0.05, 0, L), np.clip(dc + 0.05, 0, L)])
    d = d[(d >= 0) & (d <= L)]
    x = L / 2 - d
    ry = np.maximum(_tab(raggio, d, 0.0), 0.0)
    rz = ry * ellitticita
    if cassero:
        cq = _tab([[a, b] for a, b, _c in cassero], d)
        cw = _tab([[a, c] for a, _b, c in cassero], d)
    else:
        cq = cw = np.full_like(d, np.nan)
    # asse
    if asse is None:
        if bordo_libero is None: raise ValueError('scafo_sottomarino: serve `asse` o `bordo_libero`')
        zc = np.full_like(d, float(bordo_libero) - float(rz.max()))
    elif isinstance(asse, (int, float)):
        zc = np.full_like(d, float(asse))
    else:
        zc = _tab(asse, d, np.nan); zc = np.where(np.isnan(zc), np.nanmean(zc), zc)
    zc = zc + x * np.tan(np.radians(assetto)) + dz
    cq = cq + dz
    # sezioni: funzione radiale rho(phi) attorno all'asse
    phi = np.linspace(-np.pi, np.pi, int(punti_sezione), endpoint=False) + np.pi / int(punti_sezione)
    c, s = np.cos(phi), np.sin(phi)
    n = float(esponente_sezione); p = float(spigolo_cassero)
    P = np.zeros((len(d), len(phi), 3))
    for i in range(len(d)):
        if ry[i] <= 1e-6:
            rho = np.zeros_like(phi)
        else:
            rho = 1.0 / ((np.abs(c) / ry[i]) ** n + (np.abs(s) / rz[i]) ** n) ** (1 / n)
            if not np.isnan(cq[i]):
                h = cq[i] - zc[i]
                if h > 0:
                    w = max(cw[i], 1e-3)
                    rc = np.where(s > 0, 1.0 / ((np.abs(c) / w) ** p + (np.abs(s) / h) ** p + 1e-30) ** (1 / p), 0.0)
                    rho = np.maximum(rho, rc)
                    taglio = np.where(s > 1e-9, h / np.maximum(s, 1e-9), np.inf)
                    rho = np.minimum(rho, taglio)
        P[i, :, 0] = x[i]
        P[i, :, 1] = rho * c
        P[i, :, 2] = zc[i] + rho * s
    m = kit._mesh(_anelli(P), mat)
    return taglia_galleggiamento(m) if taglia else m

# ------------------------------------------------------------------ profili alari e piante
def _naca(t):
    """Spessore NACA 00xx normalizzato a 1 (massimo al 30% della corda), t in 0..1."""
    t = np.clip(t, 0, 1)
    y = 0.2969 * np.sqrt(t) - 0.1260 * t - 0.3516 * t ** 2 + 0.2843 * t ** 3 - 0.1036 * t ** 4
    return y / 0.15

def _pianta(t, forma, esponente):
    if forma == 'naca': return _naca(t)
    # superellisse (2 = ellisse, 4-8 = ovale squadrato a fianchi paralleli)
    return np.clip(1 - np.abs(2 * t - 1) ** esponente, 0, 1) ** (1 / esponente)

def _contorno_pianta(xf, xr, w, forma, esponente, n):
    """Punti (x, y) del contorno chiuso di una pianta da xf (fronte, x maggiore) a xr (retro)."""
    t = (1 - np.cos(np.linspace(0, np.pi, n // 2 + 1))) / 2          # 0 = fronte, 1 = retro, addensato agli estremi
    xs = xf + (xr - xf) * t
    ys = w * _pianta(t, forma, esponente)
    sx = np.c_[xs, ys]                                                   # lato sinistro (y > 0), fronte -> retro
    dx = np.c_[xs[::-1], -ys[::-1]][1:-1]                               # lato dritto, retro -> fronte
    return np.r_[sx, dx]

# ------------------------------------------------------------------ vela
def vela(livelli, pianta='naca', esponente=2.0, punti=28, y=0.0, piani=(), alberi=(), dz=0.0, mat='scafo',
         mat_alberi='scuro', **_):
    """Falsatorre (vela) a livelli orizzontali, dal basso verso l'alto.

    livelli: [[z, x_fronte, x_retro, semilarghezza], ...] (x anche come {prua: d}); il livello piu' basso va
             messo sotto la coperta (si unisce allo scafo), l'ultimo e' il cielo della vela (chiuso).
             Fronte inclinato, cielo arrotondato o raccordo alla base (Victor III) si ottengono con piu' livelli.
    pianta: 'naca' (profilo alare: 688, Victor) o 'ovale' (superellisse con `esponente`: Kilo, Foxtrot).
    piani: timoni di vela, [[x, z, apertura, corda, corda_estremita, spessore, freccia], ...]: bordo d'attacco
           alla radice in x (anche {prua: d}), quota, y dell'estremita' dall'asse, corde alla radice e
           all'estremita', spessore massimo, arretramento del bordo d'attacco all'estremita'.
    alberi: periscopi e alberi alzati, [[x, y, z0, z1, diametro], ...] (cilindri, materiale `mat_alberi`)."""
    L = [[float(a) for a in l] for l in livelli]
    anelli = []
    for z, xf, xr, w in L:
        xy = _contorno_pianta(max(xf, xr), min(xf, xr), w, pianta, esponente, int(punti))
        anelli.append(np.c_[xy[:, 0], xy[:, 1] + y, np.full(len(xy), z + dz)])
    A = np.array(anelli)
    tri = _anelli(A)
    cima = A[-1]; cen = cima.mean(0)
    for k in range(len(cima)):
        tri.append((cen, cima[k], cima[(k + 1) % len(cima)]))
    m = kit._mesh(tri, mat)
    for p in piani or ():
        xp, zp, ap, co, ce, sp, fr = [float(v) for v in p]
        m = m + pinna_orizzontale(xp, zp + dz, ap, co, ce, sp, fr, y=y, mat=mat)
    for a in alberi or ():
        xa, ya, z0, z1, dia = [float(v) for v in a]
        m = m + kit.cilindro(xa, ya, dia / 2, z0 + dz, z1 + dz, mat=mat_alberi, n=8)
    return taglia_galleggiamento(m)

# ------------------------------------------------------------------ pinne e timoni
def _ala(radice, estremita, corda_r, corda_e, spessore, asse_apertura, n_sez=8):
    """Superficie sottile da `radice` a `estremita` (punti 3D del bordo d'attacco), corda lungo -x, sezione a
    profilo NACA di spessore relativo `spessore`/corda nella direzione `asse_apertura` x (0, 0, 1)."""
    radice, estremita = np.asarray(radice, float), np.asarray(estremita, float)
    sp = np.asarray(asse_apertura, float); sp = sp / np.linalg.norm(sp)
    nrm = np.cross(sp, [1.0, 0, 0]);
    if np.linalg.norm(nrm) < 1e-6: nrm = np.array([0, 0, 1.0])
    nrm /= np.linalg.norm(nrm)
    t = (1 - np.cos(np.linspace(0, np.pi, n_sez))) / 2
    def sezione(le, corda):
        sup = [le + np.array([-corda * a, 0, 0]) + nrm * spessore / 2 * _naca(a) for a in t]
        inf = [le + np.array([-corda * a, 0, 0]) - nrm * spessore / 2 * _naca(a) for a in t[::-1][1:-1]]
        return np.array(sup + inf)
    S0, S1 = sezione(radice, corda_r), sezione(estremita, corda_e)
    tri = _anelli(np.array([S0, S1]))
    c1 = S1.mean(0)
    for k in range(len(S1)): tri.append((c1, S1[k], S1[(k + 1) % len(S1)]))
    return tri

def pinna_orizzontale(x, z, apertura, corda, corda_estremita=None, spessore=0.35, freccia=0.0, y=0.0, radice=0.0,
                      mat='scafo', dz=0.0, **_):
    """Coppia di pinne orizzontali simmetriche (timoni di vela, timoni orizzontali di poppa, alette):
    bordo d'attacco alla radice in x (y = +-radice), estremita' a y = +-apertura, arretrata di `freccia`."""
    ce = corda if corda_estremita is None else corda_estremita
    tri = []
    for lato in (1, -1):
        tri += _ala((x, y + lato * radice, z + dz), (x - freccia, y + lato * apertura, z + dz), corda, ce, spessore, (0, lato, 0))
    return taglia_galleggiamento(kit._mesh(tri, mat))

def pinna(x, z0, z1, corda, corda_estremita=None, spessore=0.5, freccia=0.0, y=0.0, dz=0.0, mat='scafo', **_):
    """Pinna verticale sottile (timone, deriva): bordo d'attacco in x a quota z0, estremita' a z1 arretrata di
    `freccia`, corde alla base e all'estremita'."""
    ce = corda if corda_estremita is None else corda_estremita
    m = kit._mesh(_ala((x, y, z0 + dz), (x - freccia, y, z1 + dz), corda, ce, spessore, (0, 0, 1)), mat)
    return taglia_galleggiamento(m)

def solido_rivoluzione(x, z, profilo, y=0.0, punti=20, dz=0.0, mat='scafo', **_):
    """Corpo di rivoluzione ad asse parallelo a x: `profilo` = [[d dal muso, r], ...] (d crescente verso poppa),
    muso in x, asse a quota z."""
    pr = np.asarray(profilo, float)
    phi = np.linspace(0, 2 * np.pi, int(punti), endpoint=False)
    P = np.zeros((len(pr), len(phi), 3))
    for i, (dd, r) in enumerate(pr):
        P[i, :, 0] = x - dd; P[i, :, 1] = y + r * np.cos(phi); P[i, :, 2] = z + dz + r * np.sin(phi)
    return taglia_galleggiamento(kit._mesh(_anelli(P), mat))

def pod(x, z, lunghezza, diametro, muso=0.3, coda=0.35, y=0.0, dz=0.0, mat='scafo', **_):
    """Carenatura a goccia (pod dell'array rimorchiato del Victor III, gondole): muso ellittico per la frazione
    `muso` della lunghezza, cilindro, coda conica arrotondata per `coda`. x = muso."""
    R = diametro / 2; Ln = lunghezza
    d = np.r_[np.linspace(0, muso * Ln, 7), np.linspace(muso * Ln, (1 - coda) * Ln, 3)[1:], np.linspace((1 - coda) * Ln, Ln, 6)[1:]]
    r = np.where(d < muso * Ln, R * np.sqrt(np.clip(1 - (1 - d / (muso * Ln)) ** 2, 0, 1)), R)
    m = d > (1 - coda) * Ln
    tt = (d[m] - (1 - coda) * Ln) / (coda * Ln)
    r[m] = R * (1 - tt ** 1.6) ** 0.8
    return solido_rivoluzione(x, z, np.c_[d, r], y=y, dz=dz, mat=mat)

def timone_verticale(x, z0, z1, corda, corda_estremita=None, spessore=0.6, freccia=0.0, pod=None, orizzontali=None,
                     z_asse=None, muso_pod=0.3, coda_pod=0.35, dz=0.0, mat='scafo', **_):
    """Timone superiore di poppa (pinna verticale emergente) con pod facoltativo in cima e timoni orizzontali.
    pod: [x del muso, z dell'asse, lunghezza, diametro] (x anche {prua: d}); `muso_pod`, `coda_pod` come in `pod`.
    orizzontali: [x, apertura, corda, corda_estremita, spessore, freccia] alla quota `z_asse` (di norma
    sommersi in emersione: tagliati via, servono per le varianti a fior d'acqua o in bacino)."""
    m = pinna(x, z0, z1, corda, corda_estremita, spessore, freccia, dz=dz, mat=mat)
    if pod:
        px, pz, pl, pd = [float(v) for v in pod]
        m = m + globals()['pod'](px, pz, pl, pd, muso=muso_pod, coda=coda_pod, dz=dz, mat=mat)
    if orizzontali and z_asse is not None:
        ox, oa, oc, oce, osp, ofr = [float(v) for v in orizzontali]
        m = m + pinna_orizzontale(ox, z_asse, oa, oc, oce, osp, ofr, mat=mat, dz=dz)
    return m

PARTI = dict(scafo_sottomarino=scafo_sottomarino, vela=vela, timone_verticale=timone_verticale, pinna=pinna,
             pinna_orizzontale=pinna_orizzontale,
             solido_rivoluzione=solido_rivoluzione, pod=pod)
