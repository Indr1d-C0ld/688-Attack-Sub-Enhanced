import json
S=json.load(open('stats.json'))
ROWS=[('U','Vento medio (m/s)','%.1f'),('U90','Vento p90 (m/s)','%.1f'),('B6','% ore ≥ Bft 6','%.0f'),('B7','% ore ≥ Bft 7','%.1f'),('B8','% ore ≥ Bft 8','%.1f'),('B10','% ore ≥ Bft 10','%.2f'),
('H','Hs media (m)','%.1f'),('H90','Hs p90 (m)','%.1f'),('H99','Hs p99 (m)','%.1f'),('Hge4','% ore Hs ≥ 4 m','%.1f'),('TP','Periodo medio onda (s)','%.1f'),('Hnull','% ore senza onda (ghiaccio/costa)','%.0f'),
('Ta','T aria media (°C)','%.1f'),('Ta10','T aria p10 (°C)','%.1f'),('SST','T superficie (proxy SST, °C)','%.1f'),('CC','Nuvolosità media (%)','%.0f'),('CC68','% ore cielo ≥ 6/8','%.0f'),('CC28','% ore cielo ≤ 2/8','%.0f'),('CL','Nubi basse medie (%)','%.0f'),
('pr01','% ore con precip. ≥ 0,1 mm/h','%.0f'),('pr2','% ore con precip. ≥ 2 mm/h','%.1f'),('snow','% ore con neve','%.1f'),('sat','% ore UR ≥ 97%','%.0f'),('adv','% ore UR≥97% e aria>mare','%.0f'),('smoke','% ore mare−aria ≥ 9 °C, aria ≤ 0','%.1f'),
('ppr_any','% ore spruzzo gelante possibile (PPR>0)','%.1f'),('ppr_mod','% ore PPR ≥ moderato','%.1f'),('ppr_hvy','% ore PPR ≥ forte','%.2f'),('icsev','% ore icing "severo" atlante','%.2f')]
M='G F M A M G L A S O N D'.split()
for t,R in S.items():
    print(f'\n#### {t} (punto ERA5 {R["lat"]})\n')
    print('| Variabile | '+' | '.join(M)+' |'); print('|---|'+'---|'*12)
    for k,lab,f in ROWS:
        vals=[R['mon'][str(m)][k] for m in range(1,13)]
        if all(v!=v or v==0 for v in vals): continue
        print(f'| {lab} | '+' | '.join(('n.v.' if v!=v else (f%v)) for v in vals)+' |')
    # direction per season
    print('\nDirezioni (% ore, settori da cui spira) N NE E SE S SW W NW:')
    for s,ms in (('DJF',(12,1,2)),('JJA',(6,7,8))):
        d=[sum(R['mon'][str(m)]['Dsect'][k] for m in ms)/3 for k in range(8)]
        print(s,' '.join('%.0f'%x for x in d))
    print('Douglas SS0..9 DJF/JJA:')
    for s,ms in (('DJF',(12,1,2)),('MAM',(3,4,5)),('JJA',(6,7,8)),('SON',(9,10,11))):
        d=[sum(R['mon'][str(m)]['SS'][k] for m in ms)/3 for k in range(10)]
        print(s,' '.join('%.1f'%x for x in d))
    for s,v in R['sea'].items():
        print(s,'ac',{k:round(x,2) for k,x in v['ac'].items()},'acH',{k:round(x,2) for k,x in v['ach'].items()})
        print('   g7',v['g7'],'g8',v['g8'],'g10',v['g10'],'h4',v['h4'])
        print('   lag',v['lag'],'hsU',v['hsU'],'diurn',v['diurn'],'troughs',v['troughs_per_season'],v['interval_days'])
        print('   markov',v['markov'],v['markov_n'])
