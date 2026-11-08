"""Scarica da ERDDAP NOAA i ritagli di ETOPO 2022 (15") delle basi fuori da tutti i teatri, per le loro carte (carte_basi.py).

Produce data/basi/<base>.nc. Download autorizzato dall'utente il 06/11/2026, solo per queste tre basi, circa 5 MB in tutto.
Si ferma se un file viene molto piu' grande del previsto. ETOPO 2022 e' di pubblico dominio (NOAA), come per i teatri.

Uso: fetch_etopo_basi.py <radice del progetto>
"""
import math, os, sys, urllib.request

ERDDAP = 'https://coastwatch.pfeg.noaa.gov/erddap/griddap/ETOPO_2022_v1_15s.nc'
BASI = {   # lat, lon, mezza larghezza in gradi di longitudine, MB previsti
    'keflavik': (63.98, -22.61, 2.3, 2.1),
    'vladivostok': (43.11, 131.88, 1.4, 1.3),
    'rybachiy': (52.92, 158.49, 1.7, 1.6),
}
TOLLERANZA = 2.0   # oltre il doppio del previsto si ferma

if __name__ == '__main__':
    radice = sys.argv[1]
    os.makedirs(f'{radice}/data/basi', exist_ok=True)
    for b, (lat, lon, dlon, mb) in BASI.items():
        dst = f'{radice}/data/basi/{b}.nc'
        if os.path.exists(dst):
            print(b, 'gia\' presente')
            continue
        u = f'{ERDDAP}?z[({lat - 1.0:.3f}):1:({lat + 1.0:.3f})][({lon - dlon:.3f}):1:({lon + dlon:.3f})]'
        with urllib.request.urlopen(u, timeout=300) as r:
            lunghezza = r.headers.get('Content-Length')
            if lunghezza and int(lunghezza) > TOLLERANZA * mb * 1e6:
                sys.exit(f'{b}: {int(lunghezza) / 1e6:.1f} MB invece di {mb}: mi fermo')
            dati = r.read(int(TOLLERANZA * mb * 1e6) + 1)
            if len(dati) > TOLLERANZA * mb * 1e6:
                sys.exit(f'{b}: oltre {TOLLERANZA * mb:.1f} MB invece di {mb}: mi fermo')
        with open(dst + '.part', 'wb') as f:
            f.write(dati)
        os.replace(dst + '.part', dst)
        print(b, f'{len(dati) / 1e6:.2f} MB')
