"""Scarica da NOAA NCEI (OPeNDAP) il decennio 1985-1994 del World Ocean Atlas 2023 per i teatri: l'epoca del gioco,
al posto della media 1955-2022 (decav) che porta dentro il riscaldamento dei decenni dopo. Solo ritagli: t_an e s_an
nei riquadri dei teatri (teatri.yaml), come fetch_woa.py.

Per ogni teatro in data/teatri/<id>/:
  woa23_8594_mensile_1deg.nc   t_an, s_an per mese 1..12, 0..1500 m, griglia 1° (il decennio a 0.25° non ha i mesi)
  woa23_8594_stagionale.nc     t_an, s_an per stagione (inverno, primavera, estate, autunno), 0..5500 m, griglia 0.25°

Poi, per ogni teatro, ssp_mensile_8594.json: lo stesso formato di ssp_mensile.json (fetch_woa.py), dal mensile decav a
0.25° corretto con la differenza stagionale «8594 meno decav» (stessa griglia 0.25°), cosi' il mese e la risoluzione
restano quelli del decav e l'epoca diventa quella del gioco. suono.py lo preferisce quando c'e'.

Uso: python -I fetch_woa_8594.py <radice> [--max-mb N]. Si ferma se un file supera N MB (predefinito 3): la stima e'
0,05-0,2 MB per il mensile e 0,4-1,2 per lo stagionale.
"""
import os, sys, json, warnings, numpy as np, xarray as xr, yaml

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from fetch_woa import sound_speed, MIN_CELLE
STAGIONE = {12: 0, 1: 0, 2: 0, 3: 1, 4: 1, 5: 1, 6: 2, 7: 2, 8: 2, 9: 3, 10: 3, 11: 3}   # come suono.py
FONTE = ('NOAA World Ocean Atlas 2023, decennio 1985-1994: mensile decav 0.25 gradi corretto con la differenza '
         'stagionale 8594 - decav')

BASE = 'https://www.ncei.noaa.gov/thredds-ocean/dodsC/woa23/DATA/{v}/netcdf/8594/{g}/woa23_8594_{c}{p:02d}_{r}.nc'
VAR = {'t': 'temperature', 's': 'salinity'}
GRIGLIA = {'1deg': ('1.00', '01'), '025': ('0.25', '04')}

def subset(var, period, bbox, griglia):
    la0, la1, lo0, lo1 = bbox
    g, r = GRIGLIA[griglia]
    ds = xr.open_dataset(BASE.format(v=VAR[var], g=g, c=var, p=period, r=r), decode_times=False)
    # a 1 grado un riquadro stretto (Gibilterra) puo' non contenere centri di cella: si allarga di mezza cella
    m = 0.5 if griglia == '1deg' else 0.0
    return ds[f'{var}_an'].sel(lat=slice(la0 - m, la1 + m), lon=slice(lo0 - m, lo1 + m)).isel(time=0).drop_vars('time').load()

def scrivi(f, ds, max_mb):
    ds.to_netcdf(f)
    mb = os.path.getsize(f) / 1e6
    print(f'  {os.path.basename(f)}: {mb:.2f} MB')
    if mb > max_mb:
        sys.exit(f'{f}: {mb:.2f} MB, oltre il limite di {max_mb} MB: fermo')

def ssp_corretto(d):
    """ssp_mensile_8594.json: le mediane del teatro per mese, come fetch_woa.py, sul mensile corretto."""
    mm = xr.open_dataset(f'{d}/woa23_mensile.nc', decode_times=False)
    sa = xr.open_dataset(f'{d}/woa23_stagionale.nc', decode_times=False)
    sb = xr.open_dataset(f'{d}/woa23_8594_stagionale.nc', decode_times=False)
    nz = mm.sizes['depth']
    prof = {}
    for m in range(1, 13):
        s = STAGIONE[m]
        t = mm['t_an'].sel(mese=m) + (sb.t_an.isel(stagione=s) - sa.t_an.isel(stagione=s)).isel(depth=slice(0, nz)).values
        sl = mm['s_an'].sel(mese=m) + (sb.s_an.isel(stagione=s) - sa.s_an.isel(stagione=s)).isel(depth=slice(0, nz)).values
        c, cm = sound_speed(t, sl)
        valid = (~np.isnan(t.values)).sum(axis=(1, 2))
        with warnings.catch_warnings():
            warnings.simplefilter('ignore', RuntimeWarning)
            mean = lambda a: [None if (np.isnan(x) or n < MIN_CELLE) else round(float(x), 3)
                              for x, n in zip(np.nanmedian(a, axis=(1, 2)), valid)]
            prof[m] = dict(T=mean(t.values), S=mean(sl.values), c_teos10=mean(c), c_mackenzie=mean(cm))
    teatro = os.path.basename(d)
    json.dump(dict(teatro=teatro, fonte=FONTE, profondita_m=[float(x) for x in mm.depth.values], mesi=prof),
              open(f'{d}/ssp_mensile_8594.json', 'w'), ensure_ascii=False)
    print(f'  ssp_mensile_8594.json')

if __name__ == '__main__':
    root = sys.argv[1]
    max_mb = float(sys.argv[sys.argv.index('--max-mb') + 1]) if '--max-mb' in sys.argv else 3.0
    cfg = yaml.safe_load(open(f'{root}/tools/pipeline/teatri.yaml'))
    for th in cfg['teatri']:
        d = f'{root}/data/teatri/{th["id"]}'; bb = th['bbox']
        print(th['id'])
        f_m = f'{d}/woa23_8594_mensile_1deg.nc'
        if not os.path.exists(f_m):
            T = xr.concat([subset('t', m, bb, '1deg') for m in range(1, 13)], dim='mese')
            S = xr.concat([subset('s', m, bb, '1deg') for m in range(1, 13)], dim='mese')
            scrivi(f_m, xr.Dataset({'t_an': T, 's_an': S}).assign_coords(mese=np.arange(1, 13)), max_mb)
        f_s = f'{d}/woa23_8594_stagionale.nc'
        if not os.path.exists(f_s):
            T = xr.concat([subset('t', p, bb, '025') for p in range(13, 17)], dim='stagione')
            S = xr.concat([subset('s', p, bb, '025') for p in range(13, 17)], dim='stagione')
            scrivi(f_s, xr.Dataset({'t_an': T, 's_an': S}).assign_coords(
                stagione=['inverno', 'primavera', 'estate', 'autunno']), max_mb)
        ssp_corretto(d)
