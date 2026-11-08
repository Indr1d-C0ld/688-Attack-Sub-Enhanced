import json,sys
S=json.load(open('stats.json')); S.update(json.load(open('stats_gib.json')))
ROWS=[('U','Vento medio a 10 m (m/s)','%.1f'),('B7','% ore vento ≥ Bft 7 (≥ 13,9 m/s)','%.1f'),('B8','% ore vento ≥ Bft 8 (≥ 17,2 m/s)','%.1f'),
('H','Hs media (m)','%.1f'),('H90','Hs 90° percentile (m)','%.1f'),('Hge4','% ore Hs ≥ 4 m','%.1f'),('TP','Periodo medio d\'onda (s)','%.1f'),('Hnull','% ore senza dato d\'onda (ghiaccio)','%.0f'),
('Ta','T aria media (°C)','%.1f'),('SST','T superficie mare (proxy, °C)','%.1f'),('CC68','% ore cielo coperto ≥ 6/8','%.0f'),('CL','Nubi basse medie (%)','%.0f'),
('pr01','% ore con precipitazione ≥ 0,1 mm/h','%.0f'),('snow','% ore con neve','%.1f'),('sat','% ore UR ≥ 97 % (indice nebbia, debole)','%.0f'),('smoke','% ore con mare − aria ≥ 9 °C e aria ≤ 0 °C (fumo di mare)','%.1f'),
('ppr_mod','% ore spruzzo gelante ≥ moderato (PPR ≥ 22,4)','%.1f'),('ppr_hvy','% ore spruzzo gelante forte (PPR ≥ 53,3)','%.2f')]
M='Gen Feb Mar Apr Mag Giu Lug Ago Set Ott Nov Dic'.split()
SEC='N NE E SE S SW W NW'.split()
def tab(t):
    R=S[t]; o=[]
    o.append('| Variabile | '+' | '.join(M)+' |'); o.append('|---|'+'---:|'*12)
    for k,lab,f in ROWS:
        vals=[R['mon'][str(m)][k] for m in range(1,13)]
        if all(v!=v or abs(v)<0.05 for v in vals): continue
        o.append(f'| {lab} | '+' | '.join(('n.v.' if v!=v else (f%v).replace('.',',')) for v in vals)+' |')
    o.append('')
    o.append('Direzione di provenienza del vento (% delle ore):\n')
    o.append('| Stagione | '+' | '.join(SEC)+' |'); o.append('|---|'+'---:|'*8)
    for s,ms in (('Inverno (DGF)',(12,1,2)),('Primavera (MAM)',(3,4,5)),('Estate (GLA)',(6,7,8)),('Autunno (SON)',(9,10,11))):
        d=[sum(R['mon'][str(m)]['Dsect'][k] for m in ms)/3 for k in range(8)]
        o.append(f'| {s} | '+' | '.join('%.0f'%x for x in d)+' |')
    o.append('')
    o.append('Stato del mare Douglas da Hs (% delle ore; 3 = 0,5–1,25 m, 4 = 1,25–2,5, 5 = 2,5–4, 6 = 4–6, 7 = 6–9, 8 = 9–14 m):\n')
    o.append('| Stagione | ≤ 2 | 3 | 4 | 5 | 6 | 7 | ≥ 8 |'); o.append('|---|---:|---:|---:|---:|---:|---:|---:|')
    for s,ms in (('DGF',(12,1,2)),('MAM',(3,4,5)),('GLA',(6,7,8)),('SON',(9,10,11))):
        d=[sum(R['mon'][str(m)]['SS'][k] for m in ms)/3 for k in range(10)]
        tot=sum(d) or 1
        d=[x*100/tot for x in d]
        row=[sum(d[0:3]),d[3],d[4],d[5],d[6],d[7],sum(d[8:])]
        o.append(f'| {s} | '+' | '.join(('%.1f'%x).replace('.',',') for x in row)+' |')
    return '\n'.join(o)
def eventi():
    o=['| Teatro | Stagione | Episodi ≥ Bft 7 /stagione | durata mediana (h) | p90 (h) | max (h) | Episodi ≥ Bft 8 /stagione | durata mediana (h) | max (h) | Episodi Hs ≥ 4 m /stagione | durata mediana (h) | p90 (h) | Minimi barici /stagione | intervallo medio (giorni) |','|---|---|'+'---:|'*12]
    for t,n in NAMES:
        for s in ['DJF','MAM','JJA','SON']:
            v=S[t]['sea'][s]
            g7,g8,h4=v['g7'],v['g8'],v['h4']
            f=lambda x:'–' if x!=x or x is None else ('%.0f'%x)
            o.append(f"| {n} | {SN[s]} | {('%.1f'%g7['n_per_season']).replace('.',',')} | {f(g7['durmed'])} | {f(g7['dur90'])} | {g7['durmax'] or '–'} | {('%.1f'%g8['n_per_season']).replace('.',',')} | {f(g8['durmed'])} | {g8['durmax'] or '–'} | {('%.1f'%h4['n_per_season']).replace('.',',')} | {f(h4['durmed'])} | {f(h4['dur90'])} | {('%.1f'%v['troughs_per_season']).replace('.',',')} | {('%.1f'%v['interval_days']).replace('.',',') if v['interval_days'] else '–'} |")
    return '\n'.join(o)
def acf():
    o=['| Teatro | Stagione | r(1 h) | r(6 h) | r(12 h) | r(24 h) | r(48 h) | r(72 h) | Hs r(12 h) | Hs r(24 h) | Hs r(48 h) | ritardo vento→onda (h) | Hs mediana per U 10–15 / 15–20 / 20–25 m/s | escursione diurna vento (m/s; ora max locale) |','|---|---|'+'---:|'*12]
    for t,n in NAMES:
        for s in ['DJF','JJA']:
            v=S[t]['sea'][s]; a=v['ac']; h=v['ach']; b=v['hsU']
            fb=lambda x:'–' if x[0]!=x[0] or x[1]<20 else ('%.1f'%x[0]).replace('.',',')
            o.append(f"| {n} | {SN[s]} | "+' | '.join(('%.2f'%a[k]).replace('.',',') for k in ('1','6','12','24','48','72'))+' | '+' | '.join(('%.2f'%h[k]).replace('.',',') for k in ('12','24','48'))+f" | {v['lag'][0]} | {fb(b['10'])} / {fb(b['15'])} / {fb(b['20'])} | {('%.1f'%v['diurn'][0]).replace('.',',')} ({v['diurn'][1]}h) |")
    return '\n'.join(o)
def markov():
    o=['| Teatro | Stagione | da \\ a | Calmo < 5,5 | Moderato 5,5–10,8 | Forte 10,8–17,2 | Burrasca ≥ 17,2 | quota del tempo nella classe (%) |','|---|---|---|---:|---:|---:|---:|---:|']
    lab=['Calmo','Moderato','Forte','Burrasca']
    for t,n in NAMES:
        for s in ['DJF','JJA']:
            v=S[t]['sea'][s]; N=v['markov_n']; tot=sum(N)
            for i in range(4):
                if N[i]<5: 
                    o.append(f"| {n} | {SN[s]} | {lab[i]} | – | – | – | – | {('%.1f'%(100*N[i]/tot)).replace('.',',')} |"); continue
                o.append(f"| {n} | {SN[s]} | {lab[i]} | "+' | '.join(('%.0f'%x) for x in v['markov'][i])+f" | {('%.1f'%(100*N[i]/tot)).replace('.',',')} |")
    return '\n'.join(o)
SN={'DJF':'inverno','MAM':'primavera','JJA':'estate','SON':'autunno'}
NAMES=[('faroe','1 Faroe'),('barents','2 Barents'),('gib_alboran','3 Gibilterra'),('otranto','4 Otranto'),('kunashir','5 Kunashir (Okhotsk)'),('kuril_pac','5b Curili (Pacifico)'),('celtica','6 Celtica'),('baltico','7 Baltico')]
w=sys.argv[1]
if w=='tab': print(tab(sys.argv[2]))
elif w=='ev': print(eventi())
elif w=='acf': print(acf())
elif w=='mk': print(markov())
