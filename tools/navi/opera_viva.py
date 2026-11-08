"""Opera viva delle classi del kit: carena sotto il galleggiamento, eliche, assi, timoni, cupola del sonar, bulbo.

Serve solo al modello esportato per il motore (tools/navi/esporta_modello.py), che lo disegna quando una nave
sbanda, si apprua, si spezza o si capovolge (docs/10_danni_affondamento_esplosioni.md §5.1). Calibrazione delle
foto e sprite precalcolati non la usano: restano identici.

- Navi di superficie: la carena parte dal galleggiamento vero dello scafo del kit (i vertici a z = 0 della parte
  'scafo', qualunque sia il generatore) e scende fino alla chiglia con sezioni a superellisse; il profilo della
  chiglia sale verso il dritto (piede di prua) e verso lo specchio.
- Sottomarini: lo scafo e le parti si ricostruiscono senza il taglio al galleggiamento (kit_sottomarini), piu'
  le eliche e le appendici di poppa sommerse che i YAML non descrivono (timone inferiore e piani orizzontali,
  `coda_sottomarino`).

I dati per classe (OPERA_VIVA) sono stime da schede pubbliche e foto in bacino: pescaggio dello scafo senza
cupola, numero e diametro delle eliche, timoni, cupola del sonar o bulbo. Affidabilita' bassa per le forme,
media per pescaggi ed eliche.
"""
import numpy as np
import kit
from kit import Mesh

# T: pescaggio dello scafo (m, senza cupola); D: dislocamento a pieno carico pubblicato (t): la forma delle sezioni
# si tara perche' il volume sotto il galleggiamento lo dia (acqua di mare 1,025 t/m^3); tipo: 'militare' (sezioni tonde, specchio immerso) o 'mercantile'
# (sezioni piene, fondo piatto); eliche: (numero, diametro m, pale); timoni: numero; cupola: (distanza dal dritto
# al galleggiamento, lunghezza, larghezza, quota del fondo) in m; bulbo: True per il bulbo di prua.
OPERA_VIVA = {
    'perry':      dict(T=4.5, D=4100, tipo='militare', eliche=(1, 4.9, 5), timoni=1, cupola=(38.0, 9.0, 3.0, 6.7)),
    'spruance':   dict(T=6.0, D=8040, tipo='militare', eliche=(2, 5.2, 5), timoni=2, cupola=(6.0, 18.0, 4.5, 8.8)),
    'kidd':       dict(T=6.2, D=9574, tipo='militare', eliche=(2, 5.2, 5), timoni=2, cupola=(6.0, 18.0, 4.5, 9.6)),
    'ticonderoga': dict(T=6.5, D=9800, tipo='militare', eliche=(2, 5.2, 5), timoni=2, cupola=(6.0, 18.0, 4.5, 10.2)),   # scafo della Spruance, T STIMA
    # la Type 22 (le due Batch): il Type 2016 nella cupola a prua; T e cupola STIMA (pescaggio 6,0 e 6,4 m alla cupola)
    'type22':     dict(T=5.0, D=4400, tipo='militare', eliche=(2, 4.0, 5), timoni=2, cupola=(6.6, 10.0, 3.0, 6.0)),
    # la base galleggiante Ugra (Progetto 1886): due eliche diesel (STIMA)
    'ugra':       dict(T=6.4, D=9650, tipo='militare', n_mezzo=3.0, eliche=(2, 4.0, 4), timoni=1),
    # la Knox: l'SQS-26 nella grande cupola sotto la prua; un'elica sola
    'knox':       dict(T=4.6, D=4065, tipo='militare', eliche=(1, 4.6, 5), timoni=1, cupola=(0.5, 15.0, 4.5, 7.6)),
    # la Oslo: un'elica, l'SQS-36 nella cupola a prua (pescaggio 5,5 m alla cupola, STIMA la forma)
    'oslo':       dict(T=4.4, D=1745, tipo='militare', eliche=(1, 3.4, 4), timoni=1, cupola=(14.0, 8.0, 2.6, 5.5)),
    # la Stockholm: CODAG su tre assi (la turbina al centro, i due diesel ai lati; docs/ricerca/35), eliche a quattro
    # pale, il sonar piccolo nello scafo (STIMA i diametri)
    'stockholm':  dict(T=3.3, D=372, tipo='militare', eliche=(3, 1.2, 4), timoni=2),
    'type22b2':   dict(T=5.4, D=4800, tipo='militare', eliche=(2, 4.0, 5), timoni=2, cupola=(8.0, 10.0, 3.0, 6.4)),
    'iowa':       dict(T=11.0, D=57500, tipo='militare', n_mezzo=4.0, eliche=(4, 5.5, 4), timoni=2),
    'kitty_hawk': dict(T=11.4, D=81800, tipo='militare', n_mezzo=4.5, eliche=(4, 6.4, 5), timoni=2),   # come la Nimitz (STIMA)
    'nimitz':     dict(T=11.3, D=100000, tipo='militare', n_mezzo=4.5, eliche=(4, 6.4, 5), timoni=2),
    'sacramento': dict(T=11.6, D=53600, tipo='militare', n_mezzo=4.0, eliche=(2, 5.8, 4), timoni=2),
    'krivak2':    dict(T=4.6, D=3900, tipo='militare', eliche=(2, 3.5, 4), timoni=1, cupola=(8.0, 10.0, 3.2, 7.3)),
    'kara':       dict(T=5.5, D=9700, tipo='militare', eliche=(2, 4.5, 4), timoni=1, cupola=(8.0, 14.0, 4.0, 6.7)),
    # il primo lotto delle firme nuove (dossier 31, 29/10/2026): l'Udaloy con la cupola del Polinom sotto la prua
    'udaloy':     dict(T=6.2, D=7620, tipo='militare', eliche=(2, 4.6, 5), timoni=1, cupola=(10.0, 16.0, 4.5, 7.8)),
    'kirov':      dict(T=8.0, D=24300, tipo='militare', eliche=(2, 5.5, 4), timoni=2, cupola=(14.0, 24.0, 6.0, 9.1)),
    'slava':      dict(T=7.0, D=11500, tipo='militare', eliche=(2, 5.0, 4), timoni=2, cupola=(10.0, 16.0, 5.0, 8.4)),
    'koni':       dict(T=3.6, D=1900, tipo='militare', eliche=(3, 2.5, 4), timoni=1, cupola=(20.0, 5.0, 2.0, 4.2)),
    # il Grisha III (dossier 31): tre assi, la cupola sotto la prua a 6,1 m
    'grisha3':    dict(T=3.7, D=1120, tipo='militare', eliche=(3, 2.2, 4), timoni=2, cupola=(14.0, 8.0, 2.2, 6.1)),
    'nanuchka':   dict(T=2.4, D=670, tipo='militare', eliche=(3, 1.6, 4), timoni=2),
    'hauk':       dict(T=1.6, D=148, tipo='militare', eliche=(2, 1.0, 4), timoni=2),   # modello proprio; eliche STIMA
    'agi':        dict(T=3.7, D=760, tipo='militare', eliche=(1, 2.0, 4), timoni=1),   # Okean (LINZA, 1986)
    'petroliera': dict(T=14.0, D=100000, tipo='mercantile', eliche=(1, 8.0, 5), timoni=1, bulbo=True),
    'mercantile': dict(T=9.0, D=22000, tipo='mercantile', eliche=(1, 5.5, 4), timoni=1),
    # il traghetto ro-ro, il rimorchiatore d'altura (Powhatan), la portarinfuse del traffico (STIMA sulle misure della classe)
    'traghetto':  dict(T=6.0, D=14000, tipo='mercantile', eliche=(2, 4.5, 4), timoni=2, bulbo=True),
    # la nave oceanografica (tipo Johan Hjort): un'elica a passo variabile, il timone dietro (STIMA)
    'nave_ricerca': dict(T=5.0, D=2450, tipo='mercantile', eliche=(1, 3.2, 4), timoni=1),
    'rimorchiatore': dict(T=5.0, D=1800, tipo='mercantile', n_mezzo=3.0, eliche=(2, 2.9, 4), timoni=1),
    'portarinfuse': dict(T=12.0, D=60000, tipo='mercantile', eliche=(1, 6.5, 4), timoni=1, bulbo=True),
    # il rompighiaccio della classe Kapitan Sorokin: tre eliche diesel-elettriche (STIMA il dislocamento sulle misure del traffico)
    'rompighiaccio': dict(T=9.5, D=15000, tipo='mercantile', n_mezzo=3.0, eliche=(3, 4.0, 4), timoni=1),
    'cargo':      dict(T=10.5, D=46000, tipo='mercantile', eliche=(1, 7.0, 5), timoni=1, bulbo=True),
    # peschereccio a rampa poppiera (Atlantic Peace, 1987): sezioni piene ma a fondo stellato (esponente di mezzo 4)
    # le civili col modello proprio dal 31/10/2026 (prima riscalate): i dislocamenti delle classi riscalate (STIMA)
    'bmrt':       dict(T=5.5, D=4240, tipo='mercantile', n_mezzo=4.0, eliche=(1, 3.8, 4), timoni=1),
    'cianciolo':  dict(T=5.5, D=1860, tipo='mercantile', n_mezzo=3.5, eliche=(1, 3.0, 4), timoni=1),
    # la petroliera costiera (tipo Pegasus): un'elica, il timone dietro, niente bulbo (il diametro: STIMA)
    'petroliera_costiera': dict(T=7.5, D=12100, tipo='mercantile', eliche=(1, 4.2, 4), timoni=1),
    'peschereccio_costiero': dict(T=3.0, D=360, tipo='mercantile', n_mezzo=3.0, eliche=(1, 1.6, 4), timoni=1),
    'peschereccio': dict(T=5.0, D=2000, tipo='mercantile', n_mezzo=4.0, eliche=(1, 3.2, 4), timoni=1, bulbo=True),
    # sottomarini: scafo e appendici senza taglio; eliche (numero, diametro, pale) sull'asse della coda; coda:
    # appendici di poppa sommerse (coda_sottomarino)
    'los_angeles': dict(sottomarino=True, eliche=(1, 6.7, 7), coda=dict(inferiore=5.2, piani=5.6, piastre=2.4)),
    'victor3':    dict(sottomarino=True, eliche=(1, 6.0, 4), coda=dict(inferiore=5.0, piani=5.8)),
    'victor1':    dict(sottomarino=True, eliche=(1, 5.6, 5), coda=dict(inferiore=4.6, piani=5.4)),   # 5 pale (dossier 31)
    'kilo':       dict(sottomarino=True, eliche=(1, 4.0, 6), coda=dict(inferiore=3.2, piani=4.6,
                       timone=dict(x={'prua': 65.5}, z0=-1.25, z1=2.2, corda=4.5, corda_estremita=3.0, spessore=0.5,
                                   freccia=1.5))),
    # il Type 206A: coda a croce con un'elica sola (STIMA in proporzione)
    'type206':    dict(sottomarino=True, eliche=(1, 2.3, 5), coda=dict(inferiore=1.6, piani=2.2, x={'prua': 45.5},
                       corda=2.6, corda_estremita=1.6, freccia=0.6)),   # il timone superiore e' nel file della classe
    # la Näcken: un'elica, la coda a X resa come quella a croce (STIMA)
    'nacken':     dict(sottomarino=True, eliche=(1, 3.0, 5), coda=dict(inferiore=2.4, piani=3.0, x={'prua': 45.5},
                       corda=2.4, corda_estremita=1.6, freccia=0.6)),   # il braccio superiore e' nel file della classe
    'alfa':       dict(sottomarino=True, eliche=(1, 5.5, 5), coda=dict(inferiore=5.0, piani=5.5)),
    'akula':      dict(sottomarino=True, eliche=(1, 7.0, 7), coda=dict(inferiore=6.0, piani=7.5)),
    'sierra':     dict(sottomarino=True, eliche=(1, 6.5, 7), coda=dict(inferiore=5.5, piani=7.0)),   # le pale STIMA (dossier 31)
    # il primo lotto delle firme nuove (29/10/2026): due assi con eliche a 5 pale (dossier 31), la coda STIMA
    'delta3':     dict(sottomarino=True, eliche=(2, 4.5, 5), coda=dict(inferiore=6.0, piani=7.0, x={'prua': 145.0},
                       corda=6.0, corda_estremita=4.0, freccia=1.5)),
    # l'Ohio (dossier 32): un asse, elica a 7 pale; la coda STIMA, il timone superiore del modello
    'ohio':       dict(sottomarino=True, eliche=(1, 7.0, 7), coda=dict(inferiore=5.5, piani=7.5, x={'prua': 162.0},
                       corda=5.0, corda_estremita=2.8, freccia=1.2)),
    'oscar2':     dict(sottomarino=True, eliche=(2, 5.5, 7), coda=dict(inferiore=7.0, piani=9.5, x={'prua': 132.0},
                       corda=8.0, corda_estremita=4.5, freccia=3.0)),
    'charlie2':   dict(sottomarino=True, eliche=(1, 5.5, 5), coda=dict(inferiore=5.0, piani=5.5)),
    'typhoon':    dict(sottomarino=True, eliche=(2, 5.5, 7), coda=dict(inferiore=8.0, piani=12.5, x={'prua': 164.0},
                       corda=7.0, corda_estremita=5.0, freccia=1.5, solo_piani=True)),
    'foxtrot':    dict(sottomarino=True, eliche=(3, 2.5, 4), coda=dict(inferiore=3.5, piani=4.5)),
    'sturgeon':   dict(sottomarino=True, eliche=(1, 5.5, 7), coda=dict(inferiore=4.8, piani=5.0)),
    'trafalgar':  dict(sottomarino=True, eliche=(1, 5.0, 7), coda=dict(inferiore=5.5, piani=5.5)),
}

# ------------------------------------------------------------------ galleggiamento e carena
def galleggiamento(mesh):
    """Stazioni del galleggiamento della parte 'scafo': x crescenti e semilarghezza a z = 0."""
    sel = np.array([p == 'scafo' for p in mesh.parte])
    V = mesh.tri[sel].reshape(-1, 3)
    V = V[np.abs(V[:, 2]) < 1e-6]
    if len(V) == 0: raise ValueError('opera viva: lo scafo non ha vertici al galleggiamento')
    xs = np.round(V[:, 0], 4)
    ux = np.unique(xs)
    yw = np.array([np.abs(V[xs == x, 1]).max() for x in ux])
    return ux, yw

def _profili(ux, T, tipo, n_mezzo):
    """Quota della chiglia (positiva verso il basso) ed esponente della sezione per ogni stazione."""
    xp, xa = ux.max(), ux.min(); L = xp - xa
    if tipo == 'mercantile':
        piede, risalita, immerso, n0 = 0.03, 0.07, 0.0, n_mezzo or 8.0
    else:
        piede, risalita, immerso, n0 = 0.10, 0.25, 0.15, n_mezzo or 2.6
    tb = np.clip((xp - ux) / (piede * L), 0, 1)
    ts = np.clip((ux - xa) / (risalita * L), 0, 1)
    fb = np.sqrt(1 - (1 - tb) ** 2)                       # piede di prua a quarto d'ellisse
    fs = immerso + (1 - immerso) * np.sqrt(1 - (1 - ts) ** 2)
    prof = T * np.minimum(fb, fs)
    # sezioni piu' a V verso le estremita'
    tn = np.minimum(np.clip((xp - ux) / (0.3 * L), 0, 1), np.clip((ux - xa) / (0.25 * L), 0, 1))
    n = 1.6 + (n0 - 1.6) * tn
    return prof, n

def _area_sezione(w, d, e, k=np.linspace(0, 1, 201)):
    return 2 * w * d * np.trapezoid(np.clip(1 - k ** e, 0, 1) ** (1 / e), k)

def taratura(mesh, T, tipo, n_mezzo, D):
    """Fattore della pienezza delle sezioni (esponente n -> 1 + (n - 1) f) perche' la carena sposti D tonnellate;
    restituisce (f, rapporto ottenuto)."""
    ux, yw = galleggiamento(mesh)
    prof, n = _profili(ux, T, tipo, n_mezzo)
    dx = np.gradient(ux)
    vol = lambda f: sum(_area_sezione(w, d, 1 + (e - 1) * f) * h for w, d, e, h in zip(yw, prof, n, dx) if d > 0)
    obiettivo = D / 1.025
    lo, hi = 0.02, 4.0
    if vol(lo) > obiettivo: return lo, vol(lo) / obiettivo
    if vol(hi) < obiettivo: return hi, vol(hi) / obiettivo
    for _ in range(40):
        mid = (lo + hi) / 2
        lo, hi = (mid, hi) if vol(mid) < obiettivo else (lo, mid)
    return (lo + hi) / 2, vol((lo + hi) / 2) / obiettivo

def carena(mesh, T, tipo='militare', n_mezzo=None, punti=10, D=None):
    """Carena dal galleggiamento dello scafo alla chiglia (materiale 'carena'), con lo specchio immerso. Con D
    (dislocamento, t) la pienezza delle sezioni e' tarata sul dislocamento."""
    ux, yw = galleggiamento(mesh)
    prof, n = _profili(ux, T, tipo, n_mezzo)
    if D:
        f, _ = taratura(mesh, T, tipo, n_mezzo, D)
        n = 1 + (n - 1) * f
    k = np.linspace(0, 1, punti + 1)
    S = []   # per stazione: punti della sezione di dritta (y < 0) dal galleggiamento alla chiglia
    for x, w, d, e in zip(ux, yw, prof, n):
        z = -d * k
        y = w * np.clip(1 - k ** e, 0, 1) ** (1 / e)
        S.append(np.stack([np.full_like(k, x), -y, z], 1))
    tri = []
    for i in range(len(S) - 1):
        a, b = S[i], S[i + 1]
        for j in range(punti):
            for s in (1, -1):
                f = np.array([1, s, 1])
                tri += kit._quad(a[j] * f, b[j] * f, b[j + 1] * f, a[j + 1] * f)
    # specchio immerso: chiusura della sezione di poppa (ventaglio dal centro)
    a = S[0]
    if prof[0] > 1e-3:
        c = np.array([ux[0], 0.0, -prof[0] / 2])
        for j in range(punti):
            for s in (1, -1):
                f = np.array([1, s, 1])
                tri.append((c, a[j] * f, a[j + 1] * f))
    return kit._mesh(tri, 'carena').nome('carena')

def _ellissoide(c, semiassi, mat='carena', n=12):
    s = kit.sfera(0, 0, 0, 1.0, mat, n=n)
    return Mesh(s.tri * np.asarray(semiassi, float) + np.asarray(c, float), s.mat)

def elica(x, y, z, diametro, pale=5, mat='elica'):
    """Elica ferma nel piano y-z: mozzo e pale a ventaglio (un quinto del diametro al mozzo)."""
    R = diametro / 2; r0 = 0.18 * R
    m = kit.cilindro(0, 0, r0 * 1.1, -0.6, 0.6, mat, n=10).ruota_y(90).sposta(x, y, z)
    larg = np.pi / pale * 0.9
    for p in range(pale):
        a0 = 2 * np.pi * p / pale
        bordo = []
        for t in np.linspace(0, 1, 7):        # contorno della pala: ellisse fra mozzo e punta
            r = r0 + (R - r0) * t
            w = larg * np.sqrt(max(1 - (2 * t - 1) ** 2, 0.0)) * 0.5 + 0.05
            bordo.append((r, a0 - w)); bordo.insert(0, (r, a0 + w))
        P = [np.array([x + 0.15 * np.sin(a), y + r * np.cos(a), z + r * np.sin(a)]) for r, a in bordo]
        c = np.mean(P, axis=0)
        tri = [(c, P[i], P[(i + 1) % len(P)]) for i in range(len(P))]
        m = m + kit._mesh(tri, mat)
    return m.nome('elica')

def appendici(mesh, d):
    """Eliche con assi, timoni, cupola del sonar e bulbo per una nave di superficie."""
    ux, yw = galleggiamento(mesh)
    xp, xa = ux.max(), ux.min(); L = xp - xa; B = 2 * yw.max(); T = d['T']
    m = Mesh()
    ne, De, pale = d.get('eliche', (1, 0.6 * T, 4))
    if ne == 1: ys = [0.0]
    elif ne == 2: ys = [-0.27 * B, 0.27 * B]
    elif ne == 3: ys = [-0.27 * B, 0.0, 0.27 * B]
    else: ys = [-0.33 * B, -0.15 * B, 0.15 * B, 0.33 * B]
    xe = xa + 0.06 * L if d['tipo'] == 'militare' else xa + 0.025 * L
    ze = -max(0.55 * T, De / 2 + 0.3)
    for i, y in enumerate(ys):
        xi = xe + (0.05 * L if ne == 4 and abs(y) < 0.2 * B else 0.0)   # eliche interne piu' a prua
        m = m + elica(xi, y, ze, De, pale)
        if y != 0.0 or d['tipo'] == 'militare':   # asse con braccio fino allo scafo
            m = m + kit.asta((xi, y, ze), (xi + 0.16 * L, y * 0.55, -0.6 * T), max(0.35, De * 0.08), 'scuro').nome('asse')
    nt = d.get('timoni', 1)
    yt = [0.0] if nt == 1 else [-0.27 * B, 0.27 * B]
    xt = xa + (0.02 if d['tipo'] == 'militare' else 0.008) * L
    corda = 0.035 * L if d['tipo'] == 'militare' else 0.03 * L
    for y in yt:
        m = m + kit.scatola(xt - corda / 2, xt + corda / 2, y - corda * 0.08, y + corda * 0.08, -0.95 * T, -0.1 * T,
                            'carena').nome('timone')
    if d.get('cupola'):
        dist, lung, larg, fondo = d['cupola']
        xc = xp - dist
        prof, _ = _profili(ux, T, d['tipo'], d.get('n_mezzo'))
        alto = float(np.interp(xc, ux, prof)) * 0.7      # la cupola parte dentro lo scafo
        h = (fondo - alto) / 2
        m = m + _ellissoide((xc, 0, -(alto + h)), (lung / 2, larg / 2, h)).nome('cupola_sonar')
    if d.get('bulbo'):
        m = m + _ellissoide((xp + 0.005 * L, 0, -0.6 * T), (0.045 * L, 0.11 * B, 0.3 * T)).nome('bulbo')
    return m

# ------------------------------------------------------------------ sottomarini
def eliche_sottomarino(mesh, d):
    """Eliche sulla coda dello scafo (punto piu' a poppa della parte 'scafo')."""
    sel = np.array([p == 'scafo' for p in mesh.parte])
    V = mesh.tri[sel].reshape(-1, 3)
    coda = V[V[:, 0] <= V[:, 0].min() + 0.5]
    xa, za = float(coda[:, 0].min()), float(coda[:, 2].mean())
    B = 2 * float(np.abs(V[:, 1]).max())
    ne, De, pale = d['eliche']
    if ne == 1:
        return elica(xa - 0.8, 0.0, za, De, pale)
    ys = [-0.3 * B, 0.3 * B] if ne == 2 else [-0.3 * B, 0.0, 0.3 * B]
    m = Mesh()
    for y in ys:
        m = m + elica(xa + 0.08 * (V[:, 0].max() - xa), y, za - 0.1 * B, De, pale)
    return m

def coda_sottomarino(mesh, d, cl):
    """Appendici di poppa sommerse a croce (+): timone inferiore, due piani orizzontali all'altezza dell'asse e, per
    il 688, le piastre verticali alle loro estremita'. Nei YAML c'e' solo il timone superiore, che emerge.

    La pianta viene dal timone superiore della classe (prima parte `timone_verticale`, o `coda['timone']` per il
    Kilo, dove il timone superiore e' la coda del cassero) riflessa rispetto all'asse dello scafo: bordo d'attacco,
    corde e freccia del timone superiore alla stessa distanza dall'asse. `coda`: `inferiore`, distanza
    dall'asse dell'estremita' del timone inferiore (m); `piani`, semiapertura dei piani dall'asse (m); `piastre`,
    altezza delle piastre d'estremita'; `x` ({prua: d}), `corda`, `corda_estremita`, `freccia` per una pianta
    propria (Oscar II, dove il timone superiore e' una carenatura lunga; con `solo_piani` vale solo per i piani:
    Typhoon, con i piani a poppa delle due eliche). Le radici partono
    dall'asse, dentro lo scafo. Le schede pubbliche (RAPPORTO.md) non danno timoni inferiori e piani poppieri:
    misure stimate dalle foto in bacino, affidabilita' bassa. Nessuna delle 11 classi ha la coda a X."""
    import kit_sottomarini
    c = d['coda']; XB = cl['scafo']['L'] / 2
    risolvi = lambda v: XB - v['prua'] if isinstance(v, dict) else float(v)
    t = c.get('timone') or next(p for p in cl['parti'] if p['tipo'] == 'timone_verticale')
    dz = float(t.get('dz', 0.0))
    x0, z0, z1 = risolvi(t['x']), float(t['z0']) + dz, float(t['z1']) + dz
    corda = float(t['corda']); ce = float(t.get('corda_estremita', corda)); fr = float(t.get('freccia', 0.0))
    sp = float(c.get('spessore', t.get('spessore', 0.5)))
    bordo = lambda z: x0 - fr * (z - z0) / (z1 - z0)                                     # bordo d'attacco
    corda_a = lambda z: float(np.clip(corda + (ce - corda) * (z - z0) / (z1 - z0), 0.4 * min(corda, ce), 1.3 * max(corda, ce)))
    # asse dello scafo a meta' corda della radice: fondo dello scafo piu' il semiasse verticale
    sel = np.array([p == 'scafo' for p in mesh.parte])
    V = mesh.tri[sel].reshape(-1, 3)
    ell = float(cl['scafo'].get('ellitticita', 1.0))
    def asse(x):
        w = V[np.abs(V[:, 0] - x) < 0.8]
        if len(w) == 0: w = V[np.abs(V[:, 0] - x) <= np.abs(V[:, 0] - x).min() + 0.5]
        return float(w[:, 2].min() + np.abs(w[:, 1]).max() * ell)
    def pianta(h, propria):
        """Asse, radice (x, corda) all'asse ed estremita' (x, corda) a distanza h dall'asse."""
        if propria:
            xr = risolvi(c['x'])
            return (asse(xr - float(c['corda']) / 2), xr, float(c['corda']), xr - float(c.get('freccia', 0.0)),
                    float(c.get('corda_estremita', c['corda'])))
        zc = asse(bordo(z0) - corda / 2)
        zc = asse(bordo(zc) - corda_a(zc) / 2)
        return zc, bordo(zc), corda_a(zc), bordo(zc + h), corda_a(zc + h)
    ala = kit_sottomarini._ala
    m = Mesh()
    hi = float(c['inferiore'])
    zc, a, ca, b, cb = pianta(hi, 'x' in c and not c.get('solo_piani'))
    m = m + kit._mesh(ala((a, 0, zc), (b, 0, zc - hi), ca, cb, sp, (0, 0, -1)), 'scafo').nome('timone_inferiore')
    hp = float(c['piani'])
    zc, a, ca, b, cb = pianta(hp, 'x' in c)
    tri = []
    for lato in (1, -1):
        tri += ala((a, 0, zc), (b, lato * hp, zc), ca, cb, sp, (0, lato, 0))
    m = m + kit._mesh(tri, 'scafo').nome('timoni_poppieri')
    if c.get('piastre'):
        hz = float(c['piastre']) / 2; tri = []
        for lato in (1, -1):
            tri += ala((b + 0.1 * cb, lato * hp, zc - hz), (b, lato * hp, zc + hz), cb * 1.1, cb * 0.9, 0.8 * sp, (0, 0, 1))
        m = m + kit._mesh(tri, 'scafo').nome('piastre_timoni')
    return m
