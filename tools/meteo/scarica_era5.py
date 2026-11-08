"""Scarica ERA5 orario 1981-1990 per i punti dei teatri (docs/15_meteo_e_mare.md, M0; dossier 21 §0, nota A §0.1).

Fonte: rianalisi ERA5 (ECMWF/Copernicus, Hersbach et al. 2020) tramite le API gratuite di Open-Meteo:
atmosfera https://archive-api.open-meteo.com (modello era5), onde https://marine-api.open-meteo.com (era5_ocean).
I file vanno in extracted/meteo/era5/<punto>_atm.json e <punto>_mar.json; quelli gia' scaricati si saltano.

Il punto centrale di ogni teatro (quello della nota A) ha tutte le variabili; i punti in piu' solo quelle che cambiano
nello spazio (vento, temperatura, umidita', precipitazione, neve, onde): cosi' si resta nei limiti giornalieri di
Open-Meteo (le richieste lunghe contano come piu' chiamate). Fra un punto e l'altro si aspetta; con l'errore 429 si
riprova piu' tardi.
Uso: scarica_era5.py <radice del progetto>"""
import json, os, sys, time, urllib.error, urllib.request

# punti per teatro: il primo e' il centrale della nota A (gia' scaricato), gli altri in mare dentro il riquadro di
# tools/pipeline/teatri.yaml, lontani dalla costa almeno una cella del modello d'onda (0,5 gradi) dove si puo'
PUNTI = {
    'faroe': [('faroe', 61.8, -8.0), ('faroe_n', 62.75, -6.0), ('faroe_se', 60.75, -5.0), ('faroe_nw', 63.0, -9.5),
              ('faroe_sw', 61.0, -9.5)],
    'barents': [('barents', 70.5, 35.0), ('barents_nw', 71.25, 32.0), ('barents_ne', 71.25, 38.0),
                ('barents_s', 69.75, 33.5), ('barents_se', 70.0, 38.5)],
    'gibilterra': [('gib_alboran', 36.0, -5.0), ('gib_cadice', 36.25, -7.0), ('gib_alboran_e', 35.9, -4.0),
                   ('gib_malaga', 36.4, -4.5)],
    'otranto': [('otranto', 40.5, 18.75), ('otranto_n', 41.5, 18.0), ('otranto_s', 39.75, 19.0),
                ('otranto_nw', 42.0, 17.5)],
    'kunashir': [('kunashir', 45.0, 146.5), ('kuril_pac', 43.75, 147.5), ('kunashir_w', 44.75, 145.5),
                 ('kunashir_nw', 45.4, 145.5), ('kuril_pac_s', 43.5, 147.0)],
    'celtica': [('celtica', 48.5, -10.0), ('celtica_se', 47.5, -8.0), ('celtica_nw', 49.25, -11.5),
                ('celtica_sw', 47.25, -11.5), ('celtica_ne', 49.25, -8.0)],
    'baltico': [('baltico', 54.9, 14.0), ('baltico_e', 55.5, 15.5), ('baltico_w', 54.4, 11.5),
                ('baltico_kattegat', 56.25, 11.75), ('baltico_se', 55.0, 15.75)],
}
TUTTE = ('temperature_2m,dew_point_2m,relative_humidity_2m,pressure_msl,wind_speed_10m,wind_direction_10m,'
         'wind_gusts_10m,cloud_cover,cloud_cover_low,precipitation,rain,snowfall,soil_temperature_0_to_7cm')
SPAZIALI = 'temperature_2m,relative_humidity_2m,wind_speed_10m,wind_direction_10m,precipitation,snowfall'
ONDE = 'wave_height,wave_period,wave_direction'
ONDE_SPAZIALI = 'wave_height,wave_period'
DA, A = '1981-01-01', '1990-12-31'


def url(tipo, lat, lon, variabili):
    if tipo == 'atm':
        return (f'https://archive-api.open-meteo.com/v1/archive?latitude={lat}&longitude={lon}&start_date={DA}'
                f'&end_date={A}&models=era5&wind_speed_unit=ms&hourly={variabili}')
    return (f'https://marine-api.open-meteo.com/v1/marine?latitude={lat}&longitude={lon}&start_date={DA}&end_date={A}'
            f'&models=era5_ocean&hourly={variabili}')


def scarica(radice):
    cartella = os.path.join(radice, 'extracted', 'meteo', 'era5')
    os.makedirs(cartella, exist_ok=True)
    for teatro, punti in PUNTI.items():
        for k, (nome, lat, lon) in enumerate(punti):
            for tipo in ('atm', 'mar'):
                f = os.path.join(cartella, f'{nome}_{tipo}.json')
                if os.path.exists(f) and os.path.getsize(f) > 1000:
                    continue
                var = (TUTTE if tipo == 'atm' else ONDE) if k == 0 else (SPAZIALI if tipo == 'atm' else ONDE_SPAZIALI)
                for prova in range(12):
                    try:
                        d = urllib.request.urlopen(url(tipo, lat, lon, var), timeout=600).read()
                        json.loads(d)
                        open(f, 'wb').write(d)
                        print(teatro, nome, tipo, len(d), flush=True)
                        break
                    except urllib.error.HTTPError as e:
                        attesa = 900 if e.code == 429 else 60
                        print('errore', nome, tipo, e.code, 'riprovo fra', attesa, 's', flush=True)
                        time.sleep(attesa)
                    except Exception as e:   # rete o risposta troncata
                        print('errore', nome, tipo, e, flush=True)
                        time.sleep(60)
                time.sleep(20)


if __name__ == '__main__':
    scarica(sys.argv[1] if len(sys.argv) > 1 else '.')
