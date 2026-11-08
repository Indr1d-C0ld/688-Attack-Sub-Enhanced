"""Ricollocazione delle carte originali a bassa affidabilita' (SURFB Baltico, SURFG Faroe) sul teatro reale.

georef_surf.py (tools/pipeline) confronta i livelli di profondita' con la correlazione di rango e sulle carte
stilizzate trova riquadri poco credibili. Qui si confrontano solo le terre emerse: il livello 15 della carta
originale (bianco) contro la terra di ETOPO 2022 (etopo_ampio_60s.nc del teatro). Si cerca traslazione e
scala isotropa, senza rotazione: le rotte degli script restano quelle dell'originale. Il punteggio e'
l'indice di Jaccard (intersezione / unione) delle due maschere. Poi un affinamento locale massimizza le unita'
navali iniziali delle missioni del teatro (data/missioni/SCENE*.lua, coordinate originali) che cadono in acqua
nella heightmap, tenendo il Jaccard ad almeno il 90% del migliore. Le unita' che restano a terra le sposta
missioni_lua.py, una per una, nell'acqua piu' vicina.

Uso:  ricolloca_surf.py <radice>  ->  data/teatri/... non viene toccato; risultato in re/surf/ricollocazione.json
      e confronto visivo re/surf/<MAPPA>_ricollocata.png
"""
import os, sys, glob, json, numpy as np, xarray as xr
from pyproj import Transformer
from PIL import Image

MAPPE = {'SURFB': 'baltico', 'SURFG': 'faroe'}
CELLA = 1000.0                       # griglia metrica della maschera reale (m)
M_UNITA = 1852 / 50                  # scala di gioco: metri per unita' (1 nm = 50 unita')

def originale(root, n):
    f = glob.glob(f'{root}/extracted/bathymetry/{n}_*.u8')[0]
    w, h = map(int, os.path.basename(f)[6:-3].split('x'))
    return np.frombuffer(open(f, 'rb').read(), np.uint8).reshape(h, w)   # riga 0 = nord, 1 px = 32 unita'

def maschera_reale(root, teatro):
    """Terra (z > 0) su una griglia metrica nella proiezione del teatro, ±600 km dal centro."""
    h = json.load(open(f'{root}/data/teatri/{teatro}/heightmap.json'))
    z = xr.open_dataset(f'{root}/data/teatri/{teatro}/etopo_ampio_60s.nc')['z']
    lat, lon, zz = z.latitude.values, z.longitude.values, z.values
    inv = Transformer.from_crs(h['proiezione']['proj4'], 'EPSG:4326', always_xy=True)
    r = np.arange(-600e3, 600e3 + 1, CELLA); X, Y = np.meshgrid(r, r[::-1])
    lo, la = inv.transform(X, Y)
    i = np.clip(np.searchsorted(lat, la), 0, len(lat) - 1); j = np.clip(np.searchsorted(lon, lo), 0, len(lon) - 1)
    dentro = (la >= lat[0]) & (la <= lat[-1]) & (lo >= lon[0]) & (lo <= lon[-1])
    return zz[i, j] > 0, dentro, r

def adatta(orig, terra, dentro, r):
    """Ricerca grossolana e poi fine di (centro x, centro y, scala)."""
    H, W = orig.shape
    rr, cc = np.mgrid[0:H:2, 0:W:2]
    xu = cc * 32 + 16 - 5120.0; yu = 3200.0 - (rr * 32 + 16)          # unita' dal centro della carta
    a = (orig[rr, cc] == 15).ravel(); xu = xu.ravel(); yu = yu.ravel()
    n = len(r)
    def punteggio(cx, cy, s):
        x = cx + xu * M_UNITA * s; y = cy + yu * M_UNITA * s
        j = np.round((x - r[0]) / CELLA).astype(int); i = np.round((r[-1] - y) / CELLA).astype(int)
        ok = (i >= 0) & (i < n) & (j >= 0) & (j < n)
        if ok.mean() < 0.999: return -1.0
        ok &= dentro[i.clip(0, n - 1), j.clip(0, n - 1)]
        b = terra[i.clip(0, n - 1), j.clip(0, n - 1)]
        inter = (a & b & ok).sum(); uni = ((a | b) & ok).sum()
        return inter / max(uni, 1)
    migliori = []
    for s in np.arange(0.40, 1.301, 0.05):
        for cx in np.arange(-300e3, 300e3 + 1, 8e3):
            for cy in np.arange(-300e3, 300e3 + 1, 8e3):
                p = punteggio(cx, cy, s)
                if p > 0: migliori.append((p, cx, cy, s))
    migliori.sort(reverse=True)
    fine = []
    for p0, cx0, cy0, s0 in migliori[:12]:
        for s in np.arange(s0 - 0.05, s0 + 0.051, 0.01):
            for cx in np.arange(cx0 - 8e3, cx0 + 8e3 + 1, 1e3):
                for cy in np.arange(cy0 - 8e3, cy0 + 8e3 + 1, 1e3):
                    fine.append((punteggio(cx, cy, s), cx, cy, s))
    fine.sort(reverse=True)
    return fine[0], migliori[:5]

def unita_navali(root, teatro):
    """Coordinate originali delle navi e dei sottomarini nelle missioni gia' convertite del teatro (senza ripetizioni)."""
    import re
    pts = set()
    for f in glob.glob(f'{root}/data/missioni/SCENE*.lua'):
        t = open(f).read()
        if f'teatro = "{teatro}"' not in t: continue
        for cat, ox, oy, q in re.findall(r'categoria = "(\w+)"[^\n]*\n\s+posizione = .*?originale = \{ x = (-?\d+), y = (-?\d+) \},[^\n]*\n\s+quota_m = (-?[\d.]+)', t):
            if cat in ('nave', 'sottomarino') and (ox, oy) != ('0', '6400'):   # i falsi bersagli restano
                pts.add((int(ox), int(oy), 10.0 if cat == 'nave' else float(q) + 10))
    return np.array(sorted(pts), float)

def affina(root, teatro, orig, terra, dentro, r, migliore):
    """Intorno al migliore: massimo numero di unita' con fondale sufficiente, Jaccard >= 90% del migliore."""
    from PIL import Image
    h = json.load(open(f'{root}/data/teatri/{teatro}/heightmap.json')); g = h['griglia']; c = g['cella_m']
    z = np.array(Image.open(f'{root}/data/teatri/{teatro}/heightmap_u16.png')).astype(float) * h['codifica']['scala'] + h['codifica']['offset']
    P = unita_navali(root, teatro)
    if len(P) == 0: return migliore, None
    p0, cx0, cy0, s0 = migliore
    H, W = orig.shape; rr, cc = np.mgrid[0:H:2, 0:W:2]
    xu = (cc * 32 + 16 - 5120.0).ravel(); yu = (3200.0 - (rr * 32 + 16)).ravel(); a = (orig[rr, cc] == 15).ravel(); n = len(r)
    def jac(cx, cy, s):
        x = cx + xu * M_UNITA * s; y = cy + yu * M_UNITA * s
        j = np.clip(np.round((x - r[0]) / CELLA).astype(int), 0, n - 1); i = np.clip(np.round((r[-1] - y) / CELLA).astype(int), 0, n - 1)
        ok = dentro[i, j]; b = terra[i, j]
        return (a & b & ok).sum() / max(((a | b) & ok).sum(), 1)
    def acqua(cx, cy, s):
        x = cx + (P[:, 0] - 5120) * M_UNITA * s; y = cy + (P[:, 1] - 3200) * M_UNITA * s
        col = ((x + g['larghezza'] * c / 2) // c).astype(int); row = ((g['altezza'] * c / 2 - y) // c).astype(int)
        ok = (row >= 0) & (row < z.shape[0]) & (col >= 0) & (col < z.shape[1])
        prof = np.where(ok, -z[row.clip(0, z.shape[0] - 1), col.clip(0, z.shape[1] - 1)], -1e9)
        return int((prof >= 10).sum()), int((prof >= P[:, 2]).sum())
    best = (acqua(cx0, cy0, s0), p0, cx0, cy0, s0)
    for s in np.arange(s0 - 0.04, s0 + 0.041, 0.01):
        for cx in np.arange(cx0 - 10e3, cx0 + 10e3 + 1, 500.0):
            for cy in np.arange(cy0 - 10e3, cy0 + 10e3 + 1, 500.0):
                w = acqua(cx, cy, s)
                if w <= best[0]: continue
                j = jac(cx, cy, s)
                if j >= 0.9 * p0: best = (w, j, cx, cy, s)
    (in_acqua, fondale_ok), j, cx, cy, s = best
    return (j, cx, cy, s), dict(unita=len(P), in_acqua=in_acqua, fondale_sufficiente=fondale_ok, prima=acqua(cx0, cy0, s0))

def confronto(root, n, teatro, orig, terra, r, cx, cy, s):
    """Carta originale (terra in bianco) sopra la terra reale (marrone) nel riquadro della carta."""
    H, W = orig.shape; out = np.zeros((H * 2, W * 2, 3), np.uint8)
    rr, cc = np.mgrid[0:H * 2, 0:W * 2]
    x = cx + (cc * 16 + 8 - 5120.0) * M_UNITA * s; y = cy + (3200.0 - (rr * 16 + 8)) * M_UNITA * s
    j = np.clip(np.round((x - r[0]) / CELLA).astype(int), 0, len(r) - 1); i = np.clip(np.round((r[-1] - y) / CELLA).astype(int), 0, len(r) - 1)
    b = terra[i, j]; a = orig[rr // 2, cc // 2] == 15
    out[:] = (20, 40, 90); out[b] = (150, 110, 60); out[a & ~b] = (230, 230, 230); out[a & b] = (240, 200, 120)
    os.makedirs(f'{root}/re/surf', exist_ok=True)
    Image.fromarray(out).save(f'{root}/re/surf/{n}_ricollocata.png')

if __name__ == '__main__':
    root = sys.argv[1]; ris = {}
    for n, teatro in MAPPE.items():
        orig = originale(root, n); terra, dentro, r = maschera_reale(root, teatro)
        (p, cx, cy, s), grossi = adatta(orig, terra, dentro, r)
        solo_coste = dict(jaccard=round(float(p), 3), centro_m=[float(cx), float(cy)], fattore_scala=round(float(s), 3))
        (p, cx, cy, s), unita = affina(root, teatro, orig, terra, dentro, r, (p, cx, cy, s))
        inv = Transformer.from_crs(json.load(open(f'{root}/data/teatri/{teatro}/heightmap.json'))['proiezione']['proj4'], 'EPSG:4326', always_xy=True)
        lon, lat = inv.transform(cx, cy)
        ris[n] = dict(teatro=teatro, jaccard=round(float(p), 3), centro_m=[float(cx), float(cy)], centro_lat_lon=[round(lat, 4), round(lon, 4)],
                      fattore_scala=round(float(s), 3), metri_per_unita=round(float(M_UNITA * s), 3),
                      area_nm=[round(float(204.8 * s), 1), round(float(128 * s), 1)],
                      solo_coste=solo_coste, unita_navali_affinamento=unita,
                      candidati_grossolani=[[round(float(q), 3), float(a), float(b), round(float(c), 2)] for q, a, b, c in grossi])
        confronto(root, n, teatro, orig, terra, r, cx, cy, s)
        print(n, ris[n])
    json.dump(ris, open(f'{root}/re/surf/ricollocazione.json', 'w'), indent=1, ensure_ascii=False)
