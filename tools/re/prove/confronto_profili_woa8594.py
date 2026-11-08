"""Confronto dei profili del suono: decav 1955-2022 (oggi nel motore) e decennio 1985-1994.
Corretto = mensile decav 0,25 + (stagionale 8594 - stagionale decav), stessa griglia 0,25; controllo col mensile 8594 a 1 grado."""
import sys, numpy as np, xarray as xr, warnings
warnings.filterwarnings('ignore')
root = sys.argv[1]
sys.path.insert(0, f'{root}/tools/pipeline')
from fetch_woa import sound_speed, MIN_CELLE
STAG = {12: 0, 1: 0, 2: 0, 3: 1, 4: 1, 5: 1, 6: 2, 7: 2, 8: 2, 9: 3, 10: 3, 11: 3}
DEF = {'baltico': 2, 'barents': 3, 'celtica': 2, 'faroe': 2, 'gibilterra': 8, 'kunashir': 2, 'otranto': 8}

def mediana(c, t):
    valid = (~np.isnan(t)).sum(axis=(1, 2))
    m = np.nanmedian(c, axis=(1, 2))
    m[valid < MIN_CELLE] = np.nan
    return m

def metriche(z, c):
    ok = ~np.isnan(c); z = z[ok]; c = c[ok]
    top = z <= 400
    imax = int(np.argmax(np.where(top, c, -1)))
    sld = z[imax]
    imin = int(np.argmin(c))
    return dict(c0=c[0], sld=sld, asse=z[imin], cmin=c[imin], fondo=z[-1])

righe = []
for t, mdef in DEF.items():
    d = f'{root}/data/teatri/{t}'
    mm = xr.open_dataset(f'{d}/woa23_mensile.nc', decode_times=False)
    sa = xr.open_dataset(f'{d}/woa23_stagionale.nc', decode_times=False)
    sb = xr.open_dataset(f'{d}/woa23_8594_stagionale.nc', decode_times=False)
    m1 = xr.open_dataset(f'{d}/woa23_8594_mensile_1deg.nc', decode_times=False)
    nz = mm.sizes['depth']
    z = mm.depth.values.astype(float)
    for mese in (mdef, 2 if mdef == 8 else 8):
        s = STAG[mese]
        T = mm.t_an.sel(mese=mese); S = mm.s_an.sel(mese=mese)
        dT = (sb.t_an.isel(stagione=s) - sa.t_an.isel(stagione=s)).isel(depth=slice(0, nz))
        dS = (sb.s_an.isel(stagione=s) - sa.s_an.isel(stagione=s)).isel(depth=slice(0, nz))
        Tc = T + dT.values; Sc = S + dS.values
        c_a, _ = sound_speed(T, S)
        c_b, _ = sound_speed(Tc, Sc)
        c_1, _ = sound_speed(m1.t_an.sel(mese=mese), m1.s_an.sel(mese=mese))
        a = metriche(z, mediana(c_a, T.values)); b = metriche(z, mediana(c_b, Tc.values)); u = metriche(z, mediana(c_1, m1.t_an.sel(mese=mese).values))
        Ta = np.nanmedian(T.values[0]); Tb = np.nanmedian(Tc.values[0])
        righe.append(f"| {t} | {mese}{' (predef.)' if mese == mdef else ''} | {Ta:.2f} → {Tb:.2f} | {a['c0']:.1f} → {b['c0']:.1f} (1°: {u['c0']:.1f}) | {a['sld']:.0f} → {b['sld']:.0f} (1°: {u['sld']:.0f}) | {a['asse']:.0f}/{a['cmin']:.1f} → {b['asse']:.0f}/{b['cmin']:.1f} |")
print('| Teatro | Mese | T superficie °C | c(0) m/s | SLD m | asse m / c min |')
print('|---|---|---|---|---|---|')
print('\n'.join(righe))
