"""Parti specifiche della portaerei classe Nimitz (CVN-68), configurazione 1989.

Coordinate del kit (destrorse, metri): x verso prua, y verso SINISTRA, z verso l'alto.
Le parti accettano gia' coordinate x risolte (i valori {prua: d} sono convertiti da costruisci.py,
anche dentro le liste).

Parti:
  scafo_profilato scafo con profilo di prua tabulato (prua svasata concava); sostituisce lo scafo del kit
  ponte_volo      lastra del ponte di volo a pianta qualsiasi (monotona lungo x), definita da stazioni
                  [x, y_dritta, y_sinistra]; con `scafo` (stazioni [x, semilarghezza]) aggiunge le
                  strutture inclinate sotto gli sbalzi, dal fianco dello scafo al bordo del ponte
  elevatore       piattaforma di un elevatore laterale (lastra sottile) con il pozzo della porta
  sponsone        sponsone (piattaforma a sbalzo) con mensola inclinata sotto
  sea_sparrow     lanciatore Mk29 a otto celle (Sea Sparrow): piedistallo, affusto, cassone
  aereo           sagoma bassa di un velivolo parcheggiato (fusoliera, ali ripiegate, deriva)
  aerei           gruppo di velivoli: posizioni [[x, y, orientamento], ...]
  antenna_piana   antenna radar piana a schiera (es. SPS-48): pannello inclinato su supporto
"""
import sys
sys.path.insert(0, '/home/randolph/Claude/melchior-688/tools/navi')
import numpy as np
import kit
from kit import Mesh, scatola, cilindro, asta

def _quad(a, b, c, d):
    return [(a, b, c), (a, c, d)]

def scafo_profilato(L, B, bordo, profilo_prua, finezza_prua=40.0, specchio=0.82, svasatura=0.04, stazioni=90,
                    bagnasciuga=1.0, pienezza=(0.8, 0.45), specchio_alto=None, lunghezza_poppa=30.0):
    """Variante di kit.scafo con il profilo del dritto di prua tabulato: `profilo_prua` = [[d, z], ...]
    (distanza dalla prua, quota del bordo inferiore della murata), interpolato. Serve per la prua
    svasata delle portaerei, a profilo concavo: il generatore del kit ha un dritto rettilineo.
    `specchio_alto`: larghezza relativa dello specchio in coperta (predefinito = `specchio`): lo specchio
    delle portaerei e' largo in alto e stretto al galleggiamento. `lunghezza_poppa`: tratto (m) su cui
    lo scafo si restringe verso lo specchio (30 m nel kit).
    Il resto (forma delle sezioni, svasatura, materiali) e' identico a kit.scafo."""
    sa = specchio if specchio_alto is None else specchio_alto
    XB = L / 2
    bd = np.array(sorted(bordo)); dp, zp = bd[:, 0], bd[:, 1]
    pp = np.array(sorted(profilo_prua)); pd, pz = pp[:, 0], pp[:, 1]
    def zc(x): return np.interp(XB - x, dp, zp)
    def sl(x, z):
        e = pienezza[0] + (pienezza[1] - pienezza[0]) * np.clip(z / max(zp.max(), 1), 0, 1)
        t = np.clip((XB - x) / finezza_prua, 0, 1) ** e
        sz = specchio + (sa - specchio) * np.clip(z / max(zp.max(), 1), 0, 1)
        p = sz + (1 - sz) * np.clip((x + XB) / lunghezza_poppa, 0, 1)
        return B / 2 * t * p * (1 + svasatura * z / max(zp.max(), 1))
    def zf(x):
        return min(float(np.interp(XB - x, pd, pz, right=0.0)), zc(x) * 0.98)
    # stazioni piu' fitte a prua, dove il profilo cambia
    xs = np.unique(np.concatenate([np.linspace(-XB, XB, stazioni), XB - pd[pd <= L]]))
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
        # sotto la prua rialzata: superficie che chiude lo scafo tra i due fianchi (altrimenti, vista da prua,
        # tra il dritto e i fianchi resterebbe un vuoto)
        a0, a1 = zf(x0), zf(x1)
        if max(a0, a1) > 0:
            tri += _quad((x0, sl(x0, a0), a0), (x1, sl(x1, a1), a1), (x1, -sl(x1, a1), a1), (x0, -sl(x0, a0), a0)); mat += ['scafo'] * 2
    z0 = zc(-XB); w0, w1 = sl(-XB, 0), sl(-XB, z0)
    tri += _quad((-XB, -w0, 0), (-XB, w0, 0), (-XB, w1, z0), (-XB, -w1, z0)); mat += ['scafo'] * 2
    return Mesh(tri, mat)

def ponte_volo(stazioni, z0, z1, scafo=None, caduta=4.0, mat='ponte', mat_fianco='sovrastruttura'):
    """Lastra del ponte di volo. `stazioni`: [[x, y_dritta, y_sinistra], ...] (y_dritta < 0 < y_sinistra),
    in qualsiasi ordine; la pianta e' interpolata linearmente tra stazioni consecutive (x crescente).
    z0, z1: quota inferiore e superiore della lastra (z1 = piano di volo).
    `scafo`: [[x, semilarghezza], ...] del fianco dello scafo alla quota z0 - caduta; dove il ponte
    sporge oltre lo scafo si aggiunge una superficie inclinata (strutture sotto lo sbalzo)."""
    st = np.array(sorted(stazioni, key=lambda s: s[0]), float)
    xs, yd, ys = st[:, 0], st[:, 1], st[:, 2]
    tri, mat_l = [], []
    for i in range(len(xs) - 1):
        xa, xb = xs[i], xs[i + 1]
        a_d, b_d, a_s, b_s = yd[i], yd[i + 1], ys[i], ys[i + 1]
        tri += _quad((xa, a_d, z1), (xb, b_d, z1), (xb, b_s, z1), (xa, a_s, z1)); mat_l += [mat] * 2
        tri += _quad((xa, a_d, z0), (xa, a_s, z0), (xb, b_s, z0), (xb, b_d, z0)); mat_l += [mat_fianco] * 2
        tri += _quad((xa, a_d, z0), (xb, b_d, z0), (xb, b_d, z1), (xa, a_d, z1)); mat_l += [mat_fianco] * 2
        tri += _quad((xa, a_s, z0), (xa, a_s, z1), (xb, b_s, z1), (xb, b_s, z0)); mat_l += [mat_fianco] * 2
    for i in (0, len(xs) - 1):
        x = xs[i]
        tri += _quad((x, yd[i], z0), (x, ys[i], z0), (x, ys[i], z1), (x, yd[i], z1)); mat_l += [mat_fianco] * 2
    m = Mesh(tri, mat_l)
    if scafo is not None:
        sc = np.array(sorted(scafo, key=lambda s: s[0]), float)
        xg = np.unique(np.concatenate([xs, sc[:, 0]]))
        xg = xg[(xg >= xs.min()) & (xg <= xs.max())]
        zc = z0 - caduta
        def bordo(x):
            return np.interp(x, xs, yd), np.interp(x, xs, ys), np.interp(x, sc[:, 0], sc[:, 1])
        t2, m2 = [], []
        for xa, xb in zip(xg[:-1], xg[1:]):
            da, sa, ha = bordo(xa); db, sb, hb = bordo(xb)
            # dritta: dal fianco (-h, zc) al bordo del ponte (yd, z0), solo dove il ponte sporge
            if min(-da - ha, -db - hb) > 0.3:
                t2 += _quad((xa, -ha, zc), (xb, -hb, zc), (xb, db, z0), (xa, da, z0)); m2 += [mat_fianco] * 2
            if min(sa - ha, sb - hb) > 0.3:
                t2 += _quad((xa, ha, zc), (xa, sa, z0), (xb, sb, z0), (xb, hb, zc)); m2 += [mat_fianco] * 2
        m = m + Mesh(t2, m2)
    return m

def elevatore(x0, x1, y0, y1, z, spessore=1.2, mat='ponte'):
    """Piattaforma di un elevatore laterale a livello del ponte di volo (sommita' a quota z)."""
    return scatola(x0, x1, min(y0, y1), max(y0, y1), z - spessore, z, mat)

PARTI = dict(scafo_profilato=scafo_profilato, ponte_volo=ponte_volo, elevatore=elevatore)
