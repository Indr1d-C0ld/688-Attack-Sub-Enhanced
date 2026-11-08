"""Scarica da NOAA NCEI (OPeNDAP) la climatologia World Ocean Atlas 2023 (decav, 0.25°) per i teatri
e calcola i profili della velocita' del suono.

Per ogni teatro in data/teatri/<id>/:
  woa23_mensile.nc     t_an, s_an per mese 1..12, 0..1500 m (57 livelli), griglia 0.25°
  woa23_stagionale.nc  t_an, s_an per stagione (inverno, primavera, estate, autunno), 0..5500 m
  ssp_mensile.json     profilo mediano del teatro per mese (livelli con almeno 3 celle valide): profondita', T, S, c (TEOS-10) e c Mackenzie

Velocita' del suono: gsw.sound_speed (TEOS-10) da salinita' assoluta e temperatura conservativa;
Mackenzie (1981) riportata per confronto con il modello del motore.
"""
import os, sys, json, numpy as np, xarray as xr, yaml, gsw

BASE = 'https://www.ncei.noaa.gov/thredds-ocean/dodsC/woa23/DATA/{v}/netcdf/decav/0.25/woa23_decav_{c}{p:02d}_04.nc'
VAR = {'t': 'temperature', 's': 'salinity'}
MIN_CELLE = 3

def subset(var, period, bbox):
    la0, la1, lo0, lo1 = bbox
    ds = xr.open_dataset(BASE.format(v=VAR[var], c=var, p=period), decode_times=False)
    return ds[f'{var}_an'].sel(lat=slice(la0, la1), lon=slice(lo0, lo1)).isel(time=0).drop_vars('time').load()

def mackenzie(T, S, D):
    return (1448.96 + 4.591 * T - 5.304e-2 * T ** 2 + 2.374e-4 * T ** 3 + 1.340 * (S - 35)
            + 1.630e-2 * D + 1.675e-7 * D ** 2 - 1.025e-2 * T * (S - 35) - 7.139e-13 * T * D ** 3)

def sound_speed(t, s):
    D = t.depth.values[:, None, None] * np.ones_like(t.values)
    lat = t.lat.values[None, :, None] * np.ones_like(t.values)
    lon = t.lon.values[None, None, :] * np.ones_like(t.values)
    p = gsw.p_from_z(-D, lat)
    SA = gsw.SA_from_SP(s.values, p, lon, lat)
    CT = gsw.CT_from_t(SA, t.values, p)
    return gsw.sound_speed(SA, CT, p), mackenzie(t.values, s.values, D)

if __name__ == '__main__':
    root = sys.argv[1]
    cfg = yaml.safe_load(open(f'{root}/tools/pipeline/teatri.yaml'))
    for th in cfg['teatri']:
        d = f'{root}/data/teatri/{th["id"]}'; bb = th['bbox']
        f_m = f'{d}/woa23_mensile.nc'
        if not os.path.exists(f_m):
            T = xr.concat([subset('t', m, bb) for m in range(1, 13)], dim='mese')
            S = xr.concat([subset('s', m, bb) for m in range(1, 13)], dim='mese')
            xr.Dataset({'t_an': T, 's_an': S}).assign_coords(mese=np.arange(1, 13)).to_netcdf(f_m)
        f_s = f'{d}/woa23_stagionale.nc'
        if not os.path.exists(f_s):
            T = xr.concat([subset('t', p, bb) for p in range(13, 17)], dim='stagione')
            S = xr.concat([subset('s', p, bb) for p in range(13, 17)], dim='stagione')
            xr.Dataset({'t_an': T, 's_an': S}).assign_coords(
                stagione=['inverno', 'primavera', 'estate', 'autunno']).to_netcdf(f_s)
        ds = xr.open_dataset(f_m, decode_times=False)
        prof = {}
        for m in range(1, 13):
            t = ds['t_an'].sel(mese=m); s = ds['s_an'].sel(mese=m)
            c, cm = sound_speed(t, s)
            valid = (~np.isnan(t.values)).sum(axis=(1, 2))
            with np.errstate(all='ignore'):
                # mediana robusta; livelli con meno di MIN_CELLE celle valide scartati (artefatti vicino al fondo)
                mean = lambda a: [None if (np.isnan(x) or n < MIN_CELLE) else round(float(x), 3)
                                  for x, n in zip(np.nanmedian(a, axis=(1, 2)), valid)]
                prof[m] = dict(T=mean(t.values), S=mean(s.values), c_teos10=mean(c), c_mackenzie=mean(cm))
        json.dump(dict(teatro=th['id'], fonte='NOAA World Ocean Atlas 2023, decav 1955-2022, 0.25 gradi',
                       profondita_m=[float(x) for x in ds.depth.values], mesi=prof),
                  open(f'{d}/ssp_mensile.json', 'w'), ensure_ascii=False)
        c1 = [x for x in prof[1]['c_teos10'] if x]; c8 = [x for x in prof[8]['c_teos10'] if x]
        print(f'{th["id"]:10s} gen c(0)={c1[0]} min={min(c1):.1f} fino a {ds.depth.values[len(c1)-1]:.0f} m  ago c(0)={c8[0]} min={min(c8):.1f}')
