"""Scarica da ERDDAP NOAA la batimetria ETOPO 2022 (15") per i teatri definiti in teatri.yaml.

Per ogni teatro produce in data/teatri/<id>/:
  etopo15s.nc         riquadro del teatro a piena risoluzione (15")
  etopo_ampio_60s.nc  riquadro allargato (margine in gradi) a 60" (passo 4), per il confronto con le mappe originali
"""
import os, sys, time, urllib.request, yaml

ERDDAP = 'https://coastwatch.pfeg.noaa.gov/erddap/griddap/ETOPO_2022_v1_15s.nc'

def url(lat0, lat1, lon0, lon1, step):
    return f'{ERDDAP}?z[({lat0}):{step}:({lat1})][({lon0}):{step}:({lon1})]'

def get(u, dst):
    if os.path.exists(dst) and os.path.getsize(dst) > 0:
        return 'presente'
    for tentativo in range(3):
        try:
            with urllib.request.urlopen(u, timeout=300) as r, open(dst + '.part', 'wb') as f:
                f.write(r.read())
            os.replace(dst + '.part', dst)
            return f'{os.path.getsize(dst) // 1024} KB'
        except Exception as e:
            print('  errore:', e, '- riprovo'); time.sleep(5)
    raise RuntimeError('download fallito: ' + u)

if __name__ == '__main__':
    root = sys.argv[1]
    margine = float(sys.argv[2]) if len(sys.argv) > 2 else 4.0
    cfg = yaml.safe_load(open(os.path.join(root, 'tools/pipeline/teatri.yaml')))
    for t in cfg['teatri']:
        la0, la1, lo0, lo1 = t['bbox']
        d = os.path.join(root, 'data/teatri', t['id']); os.makedirs(d, exist_ok=True)
        print(t['id'], 'teatro', get(url(la0, la1, lo0, lo1, 1), f'{d}/etopo15s.nc'))
        print(t['id'], 'ampio ', get(url(max(la0 - margine, -89), min(la1 + margine, 89),
                                         lo0 - 2 * margine, lo1 + 2 * margine, 4), f'{d}/etopo_ampio_60s.nc'))
