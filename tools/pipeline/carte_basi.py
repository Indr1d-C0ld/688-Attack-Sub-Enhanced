"""Le carte delle basi per il Museo: una per base, centrata sul punto, cosi' nessuna cade fuori dal riquadro di un teatro.

Prima ogni base usava la carta d'epoca del suo teatro: il riquadro di gioco e' stretto, e molte stavano fuori o sul bordo
(una scheda blu col solo cerchio rosso). Queste carte servono solo all'immagine del Museo: la simulazione non le legge.

Dati: ETOPO 2022 gia' scaricato (data/teatri/<teatro>/etopo15s.nc e etopo_ampio_60s.nc; data/basi/<base>.nc per i punti fuori
da tutti i teatri, tools/pipeline/fetch_etopo.py). Per ogni cella si prende la griglia migliore che la copre: 15", poi 60".
Proiezione azimutale equidistante centrata sulla base (WGS84), come i teatri; stesso formato .fdl (carta_epoca.scrivi_fdl).
Profondita' in metri interi e terra a passi di 10 m, come le carte d'epoca (carta_epoca, punto 6); nessun errore d'epoca.

Uscita: data/motore/basi/<base>.fdl. Stampa per ogni base la griglia usata e la terra piu' vicina al punto: una base che resta
piu' lontana di MAX_DA_TERRA_KM da terra e' un errore, salvo le ISOLE_PICCOLE, troppo piccole per la griglia.

Uso: carte_basi.py <radice del progetto> (lo chiama anche esporta_motore.py)
"""
import glob, math, os, sys
import numpy as np, xarray as xr, yaml
from pyproj import Proj
from scipy import ndimage

from carta_epoca import blocco_geo, scrivi_fdl

CELLA_M = 400.0
LARGHEZZA_M, ALTEZZA_M = 168000.0, 96000.0   # poco piu' della vista del Museo: 150 km nel riquadro 236x128
SCALA, OFFSET = 0.25, -11000.0               # la codifica dei teatri (build_heightmap)
MAX_DA_TERRA_KM = 4.0
# L'isola di Alborán: 7 ettari, sotto la griglia ETOPO. Si segna come una cella di terra nel suo punto vero
ISOLE_PICCOLE = {'alboran'}


def griglie(radice):
    """Le griglie su disco, dalla piu' fine: (dati, passo in gradi, nome)"""
    r = []
    for schema, passo in (('data/basi/*.nc', 15 / 3600), ('data/teatri/*/etopo15s.nc', 15 / 3600), ('data/teatri/*/etopo_ampio_60s.nc', 60 / 3600)):
        for f in sorted(glob.glob(f'{radice}/{schema}')):
            if '_riquadro' in f:
                continue
            z = xr.open_dataset(f)['z'].sortby('latitude').sortby('longitude')
            r.append((z, passo, os.path.relpath(f, f'{radice}/data')))
    return r


def campiona(griglie_, lon, lat):
    """z su lon, lat dalla griglia migliore che copre la cella; la griglia usata per il maggior numero di celle"""
    z = np.full(lon.shape, np.nan)
    conteggio = {}
    for g, _, nome in griglie_:
        if not np.isnan(z).any():
            break
        la, lo = g['latitude'].values, g['longitude'].values
        dentro = np.isnan(z) & (lat >= la[0]) & (lat <= la[-1]) & (lon >= lo[0]) & (lon <= lo[-1])
        if not dentro.any():
            continue
        i = (lat[dentro] - la[0]) / (la[1] - la[0])
        j = (lon[dentro] - lo[0]) / (lo[1] - lo[0])
        z[dentro] = ndimage.map_coordinates(g.values, [i, j], order=1)
        conteggio[nome] = int(dentro.sum())
    return z, conteggio


def carta_base(griglie_, lat, lon):
    nx, ny = int(LARGHEZZA_M / CELLA_M), int(ALTEZZA_M / CELLA_M)
    pj = Proj(proj='aeqd', lat_0=lat, lon_0=lon, datum='WGS84', units='m')
    xs = (np.arange(nx) + 0.5) * CELLA_M - nx * CELLA_M / 2
    ys = ny * CELLA_M / 2 - (np.arange(ny) + 0.5) * CELLA_M
    X, Y = np.meshgrid(xs, ys)
    lo, la = pj(X, Y, inverse=True)
    z, conteggio = campiona(griglie_, lo, la)
    return z, conteggio


def esporta_carte_basi(radice):
    basi = yaml.safe_load(open(f'{radice}/tools/pipeline/basi.yaml', encoding='utf-8'))
    g = griglie(radice)
    os.makedirs(f'{radice}/data/motore/basi', exist_ok=True)
    fatte, mancanti, lontane = [], [], []
    for b, v in basi.items():
        if not isinstance(v, dict) or 'lat' not in v:
            continue
        z, conteggio = carta_base(g, v['lat'], v['lon'])
        vuote = float(np.isnan(z).mean())
        if vuote > 0.02:
            mancanti.append(b)
            print(f'{b:18} NESSUNA CARTA: dati per il {100 * (1 - vuote):.0f}% della vista')
            continue
        if vuote:   # un angolo appena fuori dai dati: il valore piu' vicino, come build_heightmap
            i = ndimage.distance_transform_edt(np.isnan(z), return_distances=False, return_indices=True)
            z = z[tuple(i)]
        h, w = z.shape
        if b in ISOLE_PICCOLE:
            z[h // 2 - 1:h // 2 + 1, w // 2 - 1:w // 2 + 1] = np.maximum(z[h // 2 - 1:h // 2 + 1, w // 2 - 1:w // 2 + 1], 10.0)
        z = np.where(z >= 0, np.round(z / 10.0) * 10.0, np.minimum(np.round(z), -1.0))
        dist = ndimage.distance_transform_edt(z < 0)[h // 2, w // 2] * CELLA_M / 1000.0
        if dist > MAX_DA_TERRA_KM:
            lontane.append(b)
        u16 = np.clip(np.round((z - OFFSET) / SCALA), 0, 65535).astype(np.uint16)
        meta = {'proiezione': {'centro_lat': v['lat'], 'centro_lon': v['lon']}}
        scrivi_fdl(f'{radice}/data/motore/basi/{b}.fdl', u16, CELLA_M, SCALA, OFFSET, [blocco_geo(meta)])
        principale = max(conteggio, key=conteggio.get)
        print(f'{b:18} {principale:38} terra a {dist:4.1f} km')
        fatte.append(b)
    if lontane:
        print('LONTANE DA TERRA:', lontane)
    return fatte, mancanti, lontane


if __name__ == '__main__':
    f, m, l = esporta_carte_basi(sys.argv[1] if len(sys.argv) > 1 else '.')
    print(f'carte: {len(f)}; senza dati: {m}; lontane da terra: {l}')
