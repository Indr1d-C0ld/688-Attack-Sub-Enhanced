"""Costruisce le heightmap a 16 bit dei teatri per il motore di gioco.

Per ogni teatro di teatri.yaml:
 1. griglia locale in proiezione azimutale equidistante (pyproj, WGS84) centrata sul riquadro,
    con celle quadrate di `cella_m` metri e dimensioni pari al riquadro misurato al centro;
 2. fonte primaria: EMODnet Bathymetry DTM (WCS `emodnet__mean`, 1/16', release corrente) per i
    teatri europei, scaricata nell'area geografica coperta dalla griglia; ETOPO 2022 15" dove
    EMODnet manca e per i teatri fuori copertura;
 3. ricampionamento bilineare con sovracampionamento 2x2 e media dei campioni (anti-aliasing);
 4. uscite in data/teatri/<id>/:
      heightmap_u16.png   PNG a 16 bit, quota z [m] = valore * 0.25 - 11000 (riga 0 = nord)
      fonte_u8.png        provenienza per cella: 1 = EMODnet, 2 = ETOPO
      heightmap.json      metadati: proiezione, centro, cella, dimensioni, codifica, fonti, statistiche
      anteprima.png       immagine di controllo (colori batimetrici, terra in ocra)
    La griglia scaricata da EMODnet viene salvata in emodnet.nc con le coordinate (il GeoTIFF del
    servizio non contiene tag di georeferenziazione: la griglia si ricava dalla DescribeCoverage).
"""
import os, sys, json, datetime, urllib.request, io, time
import numpy as np, xarray as xr, yaml, tifffile
from pyproj import Proj, Geod
from scipy.ndimage import map_coordinates
from PIL import Image

WCS = ('https://ows.emodnet-bathymetry.eu/wcs?service=WCS&version=2.0.1&request=GetCoverage'
       '&coverageId=emodnet__mean&subset=Lat({la0},{la1})&subset=Long({lo0},{lo1})&format=image/tiff')
PASSO = 1 / 960            # passo della griglia EMODnet (1/16 di primo d'arco)
SCALA, OFFSET = 0.25, -11000.0
SS = 2                     # sovracampionamento per lato

def snap(v, up):
    k = np.ceil(v / PASSO) if up else np.floor(v / PASSO)
    return round(float(k * PASSO), 6)

def tassello(la0, la1, lo0, lo1):
    """Un tassello dal WCS, con fino a 4 tentativi (il server rifiuta richieste troppo grandi)."""
    u = WCS.format(la0=la0, la1=la1, lo0=lo0, lo1=lo1)
    for k in range(4):
        try:
            with urllib.request.urlopen(u, timeout=600) as r:
                a = tifffile.imread(io.BytesIO(r.read())).astype(np.float32)
            ny, nx = round((la1 - la0) / PASSO), round((lo1 - lo0) / PASSO)
            if a.shape != (ny, nx):
                raise ValueError(f'dimensioni inattese {a.shape} invece di {(ny, nx)}')
            return a
        except Exception as e:
            print(f'  tassello {la0},{la1},{lo0},{lo1}: {e} (tentativo {k + 1})', flush=True)
            time.sleep(10 * (k + 1))
    raise RuntimeError('download EMODnet fallito')

def emodnet(path, la0, la1, lo0, lo1, lato=1.0):
    """Ritaglio EMODnet allineato alla griglia nativa, scaricato a tasselli di `lato` x 2*`lato` gradi."""
    la0, la1, lo0, lo1 = snap(la0, False), snap(la1, True), snap(lo0, False), snap(lo1, True)
    if not os.path.exists(path):
        righe = []
        for a1 in np.arange(la1, la0, -lato):
            b0 = round(max(a1 - lato, la0), 6); a1 = round(float(a1), 6)
            riga = [tassello(b0, a1, round(float(o0), 6), round(min(o0 + 2 * lato, lo1), 6))
                    for o0 in np.arange(lo0, lo1, 2 * lato)]
            righe.append(np.concatenate(riga, axis=1))
        a = np.concatenate(righe, axis=0)
        lat = la1 - (np.arange(a.shape[0]) + 0.5) * PASSO          # riga 0 = nord (verificato su ETOPO)
        lon = lo0 + (np.arange(a.shape[1]) + 0.5) * PASSO
        xr.Dataset({'z': (('lat', 'lon'), a)}, coords={'lat': lat, 'lon': lon},
                   attrs={'fonte': 'EMODnet Bathymetry DTM, WCS emodnet__mean', 'licenza': 'CC-BY 4.0'}
                   ).to_netcdf(path, encoding={'z': {'zlib': True, 'complevel': 4}})
    return xr.open_dataset(path)['z']

def sample(da, lat, lon, latn='lat', lonn='lon'):
    """Interpolazione bilineare di una griglia regolare lat/lon in punti arbitrari (NaN fuori)."""
    la = da[latn].values; lo = da[lonn].values
    fi = (lat - la[0]) / (la[1] - la[0]); fj = (lon - lo[0]) / (lo[1] - lo[0])
    return map_coordinates(da.values.astype(np.float32), [fi, fj], order=1, mode='constant', cval=np.nan)

def colori(z):
    pmax = max(50.0, float(np.percentile(-z[z < 0], 99))) if (z < 0).any() else 50.0
    d = np.clip(-z, 0, pmax) / pmax          # scala di colore adattata alla profondita' del teatro
    rgb = np.stack([30 + 170 * (1 - d) ** 2, 60 + 170 * (1 - d) ** 1.5, 110 + 140 * (1 - d)], -1)
    terra = z >= 0
    h = np.clip(z, 0, 1500) / 1500
    rgb[terra] = np.stack([190 - 60 * h, 170 - 70 * h, 110 - 50 * h], -1)[terra]
    return rgb.astype(np.uint8)

def costruisci(root, t, dflt):
    d = f'{root}/data/teatri/{t["id"]}'
    la0, la1, lo0, lo1 = t['bbox']
    cella = float(t.get('cella_m', dflt['cella_m'])); fonte = t.get('batimetria', dflt['batimetria'])
    clat, clon = (la0 + la1) / 2, (lo0 + lo1) / 2
    g = Geod(ellps='WGS84')
    w = g.inv(lo0, clat, lo1, clat)[2]; h = g.inv(clon, la0, clon, la1)[2]
    nx, ny = int(round(w / cella)), int(round(h / cella))
    pj = Proj(proj='aeqd', lat_0=clat, lon_0=clon, datum='WGS84', units='m')
    # centri delle sottocelle: x da ovest a est, y da nord a sud
    xs = (np.arange(nx * SS) + 0.5) * cella / SS - nx * cella / 2
    ys = ny * cella / 2 - (np.arange(ny * SS) + 0.5) * cella / SS
    X, Y = np.meshgrid(xs, ys)
    lon, lat = pj(X, Y, inverse=True)
    et = xr.open_dataset(f'{d}/etopo15s.nc')['z']
    ze = sample(et, lat, lon, 'latitude', 'longitude')
    # gli angoli della griglia proiettata possono uscire dal ritaglio a 15": ripiego sull'area ampia a 60"
    ea = xr.open_dataset(f'{d}/etopo_ampio_60s.nc')['z'].sortby('latitude')
    ze = np.where(np.isnan(ze), sample(ea, lat, lon, 'latitude', 'longitude'), ze)
    if fonte == 'emodnet':
        m = 0.02
        em = emodnet(f'{d}/emodnet.nc', lat.min() - m, lat.max() + m, lon.min() - m, lon.max() + m)
        zp = sample(em, lat, lon)
    else:
        zp = np.full_like(ze, np.nan)
    src = np.where(np.isnan(zp), 2, 1).astype(np.uint8)   # 2 = ETOPO (15" o, ai bordi, 60")
    z = np.where(np.isnan(zp), ze, zp)
    # media dei blocchi SSxSS
    z = z.reshape(ny, SS, nx, SS).mean(axis=(1, 3))
    src = src.reshape(ny, SS, nx, SS).max(axis=(1, 3))
    buchi = int(np.isnan(z).sum())
    if buchi:  # bordi oltre i dati ETOPO scaricati: riempiti con il valore valido piu' vicino
        from scipy.ndimage import distance_transform_edt
        idx = distance_transform_edt(np.isnan(z), return_distances=False, return_indices=True)
        z = z[tuple(idx)]
    u16 = np.clip(np.round((z - OFFSET) / SCALA), 0, 65535).astype(np.uint16)
    Image.fromarray(u16).save(f'{d}/heightmap_u16.png')
    Image.fromarray(src).save(f'{d}/fonte_u8.png')
    prev = Image.fromarray(colori(z))
    prev.thumbnail((900, 900)); prev.save(f'{d}/anteprima.png')
    # verifica di andata e ritorno della codifica
    back = np.array(Image.open(f'{d}/heightmap_u16.png')).astype(np.float64) * SCALA + OFFSET
    err = float(np.abs(back - z).max())
    meta = dict(
        teatro=t['id'], nome=t['nome'], creato=datetime.date.today().isoformat(),
        proiezione=dict(proj4=pj.srs, tipo='azimutale equidistante', datum='WGS84',
                        centro_lat=clat, centro_lon=clon),
        griglia=dict(cella_m=cella, larghezza=nx, altezza=ny, estensione_m=[nx * cella, ny * cella],
                     origine='riga 0 = nord, colonna 0 = ovest; centro griglia = centro della proiezione',
                     coordinate_cella='x = (col + 0.5) * cella - larghezza*cella/2 ; y = altezza*cella/2 - (riga + 0.5) * cella'),
        codifica=dict(formato='PNG grigio 16 bit', formula='z_m = valore * 0.25 - 11000', scala=SCALA,
                      offset=OFFSET, errore_max_m=round(err, 4)),
        fonti=[dict(id=1, nome='EMODnet Bathymetry DTM (release corrente, 1/16 di primo)',
                    accesso='https://ows.emodnet-bathymetry.eu/wcs coverage emodnet__mean', licenza='CC-BY 4.0',
                    attribuzione='EMODnet Bathymetry Consortium (2024): EMODnet Digital Bathymetry (DTM)')
               ] * (fonte == 'emodnet') + [
               dict(id=2, nome='NOAA NCEI ETOPO 2022 15 secondi d\'arco', accesso='ERDDAP ETOPO_2022_v1_15s',
                    licenza='dati del governo USA, uso libero', attribuzione='NOAA NCEI (2022), doi:10.25921/fd45-gt74')],
        statistiche=dict(z_min=round(float(z.min()), 1), z_max=round(float(z.max()), 1),
                         frazione_terra=round(float((z >= 0).mean()), 4),
                         frazione_emodnet=round(float((src == 1).mean()), 4), celle_riempite_bordo=buchi))
    json.dump(meta, open(f'{d}/heightmap.json', 'w'), indent=2, ensure_ascii=False)
    return meta

if __name__ == '__main__':
    root = sys.argv[1]
    cfg = yaml.safe_load(open(f'{root}/tools/pipeline/teatri.yaml'))
    solo = sys.argv[2:]
    for t in cfg['teatri']:
        if solo and t['id'] not in solo: continue
        m = costruisci(root, t, cfg['predefiniti'])
        s = m['statistiche']; gr = m['griglia']
        print(f"{t['id']:10s} {gr['larghezza']}x{gr['altezza']} celle da {gr['cella_m']:.0f} m "
              f"({gr['estensione_m'][0]/1852:.0f}x{gr['estensione_m'][1]/1852:.0f} nm)  z {s['z_min']}..{s['z_max']} m  "
              f"terra {s['frazione_terra']:.1%}  EMODnet {s['frazione_emodnet']:.1%}  bordo riempito {s['celle_riempite_bordo']}  "
              f"errore codifica {m['codifica']['errore_max_m']} m", flush=True)
