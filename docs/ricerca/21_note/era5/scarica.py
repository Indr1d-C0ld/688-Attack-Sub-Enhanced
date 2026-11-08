import json, urllib.request, time, sys, os
P = {
 'faroe':(61.8,-8.0), 'barents':(70.5,35.0), 'gibilterra':(35.95,-5.55), 'gib_alboran':(36.0,-5.0),
 'otranto':(40.5,18.75), 'kunashir':(45.0,146.5), 'kuril_pac':(43.7,147.5),
 'celtica':(48.5,-10.0), 'baltico':(54.9,14.0)}
A='temperature_2m,dew_point_2m,relative_humidity_2m,pressure_msl,wind_speed_10m,wind_direction_10m,wind_gusts_10m,cloud_cover,cloud_cover_low,precipitation,rain,snowfall,soil_temperature_0_to_7cm'
M='wave_height,wave_period,wave_direction'
for k,(la,lo) in P.items():
  for kind,url in (('atm',f'https://archive-api.open-meteo.com/v1/archive?latitude={la}&longitude={lo}&start_date=1981-01-01&end_date=1990-12-31&models=era5&wind_speed_unit=ms&hourly={A}'),
                   ('mar',f'https://marine-api.open-meteo.com/v1/marine?latitude={la}&longitude={lo}&start_date=1981-01-01&end_date=1990-12-31&models=era5_ocean&hourly={M}')):
    f=f'{k}_{kind}.json'
    if os.path.exists(f) and os.path.getsize(f)>1000: continue
    for t in range(5):
      try:
        d=urllib.request.urlopen(url,timeout=300).read(); open(f,'wb').write(d); print(k,kind,len(d),flush=True); break
      except Exception as e:
        print('err',k,kind,e,flush=True); time.sleep(30)
    time.sleep(5)
