import json
NAMES=[('faroe','1 Faroe'),('barents','2 Barents'),('gib_alboran','3 Gibilterra'),('otranto','4 Otranto'),('kunashir','5 Kunashir'),('kuril_pac','5b Curili Pac.'),('celtica','6 Celtica'),('baltico','7 Baltico')]
B=['00–06','06–12','12–18','18–24']
print('| Teatro | Stagione | Variabile | '+' | '.join(B)+' |'); print('|---|---|---|'+'---:|'*4)
for t,n in NAMES:
    a=json.load(open(t+'_atm.json')); m=json.load(open(t+'_mar.json'))['hourly']; lon=a['longitude']; h=a['hourly']
    mon=[int(s[5:7]) for s in h['time']]; hr=[int(s[11:13]) for s in h['time']]
    for sn,ms in (('estate',(6,7,8)),('inverno',(12,1,2))):
        acc={k:[[] for _ in range(4)] for k in ('U','P','CL','RH','H')}
        for i in range(len(mon)):
            if mon[i] not in ms: continue
            b=int(((hr[i]+lon/15)%24)//6)
            if h['wind_speed_10m'][i] is not None: acc['U'][b].append(h['wind_speed_10m'][i])
            if h['precipitation'][i] is not None: acc['P'][b].append(100.0 if h['precipitation'][i]>=0.1 else 0.0)
            if h['cloud_cover_low'][i] is not None: acc['CL'][b].append(h['cloud_cover_low'][i])
            if h['relative_humidity_2m'][i] is not None: acc['RH'][b].append(100.0 if h['relative_humidity_2m'][i]>=97 else 0.0)
            if m['wave_height'][i] is not None: acc['H'][b].append(m['wave_height'][i])
        for k,lab,f in (('U','vento medio (m/s)','%.1f'),('P','% ore con precipitazione','%.0f'),('CL','nubi basse (%)','%.0f'),('RH','% ore UR ≥ 97 %','%.0f'),('H','Hs media (m)','%.2f')):
            v=[sum(x)/len(x) if x else float('nan') for x in acc[k]]
            print(f'| {n} | {sn} | {lab} | '+' | '.join((f%x).replace('.',',') for x in v)+' |')
