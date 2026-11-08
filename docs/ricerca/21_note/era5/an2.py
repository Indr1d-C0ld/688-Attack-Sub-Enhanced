import json, math, sys
from collections import defaultdict
T=['gib_alboran']
WAV={'gibilterra':'gib_alboran'}
SEAS={12:'DJF',1:'DJF',2:'DJF',3:'MAM',4:'MAM',5:'MAM',6:'JJA',7:'JJA',8:'JJA',9:'SON',10:'SON',11:'SON'}
def pct(v,p):
    v=sorted(v); 
    if not v: return float('nan')
    i=(len(v)-1)*p; a=int(i); b=min(a+1,len(v)-1); return v[a]+(v[b]-v[a])*(i-a)
def mean(v): return sum(v)/len(v) if v else float('nan')
def fr(v,c): return 100*sum(1 for x in v if c(x))/len(v) if v else float('nan')
def douglas(h):
    for i,lim in enumerate([0.0001,0.1,0.5,1.25,2.5,4,6,9,14]):
        if h<lim: return i
    return 9
out={}
for t in T:
    a=json.load(open(t+'_atm.json'))['hourly']; m=json.load(open(WAV.get(t,t)+'_mar.json'))
    lat=m['latitude']; mh=m['hourly']
    n=len(a['time']); mon=[int(s[5:7]) for s in a['time']]; hr=[int(s[11:13]) for s in a['time']]
    U=a['wind_speed_10m']; D=a['wind_direction_10m']; G=a['wind_gusts_10m']; Ta=a['temperature_2m']; Td=a['dew_point_2m']; RH=a['relative_humidity_2m']
    P=a['pressure_msl']; CC=a['cloud_cover']; CL=a['cloud_cover_low']; PR=a['precipitation']; SN=a['snowfall']; SST=a['soil_temperature_0_to_7cm']
    H=mh['wave_height']; TP=mh['wave_period']
    R={'lat':lat}
    M=defaultdict(lambda: defaultdict(list))
    for i in range(n):
        mo=mon[i]
        for key,arr in (('U',U),('G',G),('Ta',Ta),('RH',RH),('CC',CC),('CL',CL),('PR',PR),('SN',SN),('SST',SST),('H',H),('TP',TP)):
            if arr[i] is not None: M[mo][key].append(arr[i])
        M[mo]['Hnull'].append(H[i] is None)
        if None not in (Ta[i],SST[i]): M[mo]['dT'].append(SST[i]-Ta[i])
        if None not in (Ta[i],SST[i],RH[i]): 
            M[mo]['advfog'].append(RH[i]>=97 and Ta[i]>SST[i])
            M[mo]['smoke'].append(SST[i]-Ta[i]>=9 and Ta[i]<=0 )
            M[mo]['sat'].append(RH[i]>=97)
        if None not in (Ta[i],SST[i],U[i]):
            Tf=-1.7; ppr=U[i]*(Tf-Ta[i])/(1+0.3*(max(SST[i],Tf)-Tf)) if Ta[i]<Tf else 0
            M[mo]['ppr'].append(ppr)
            M[mo]['icmod'].append(Ta[i]<-2 and U[i]>=6.7)
            M[mo]['icsev'].append(Ta[i]<-9 and U[i]>=15.4)
        if PR[i] is not None:
            M[mo]['pr01'].append(PR[i]>=0.1); M[mo]['pr2'].append(PR[i]>=2.0)
            M[mo]['snowh'].append(PR[i]>=0.1 and SN[i] is not None and SN[i]>0.05)
    mt={}
    for mo in range(1,13):
        d=M[mo]; u=d['U']; h=d['H']
        mt[mo]=dict(U=mean(u),U90=pct(u,.9),B6=fr(u,lambda x:x>=10.8),B7=fr(u,lambda x:x>=13.9),B8=fr(u,lambda x:x>=17.2),B9=fr(u,lambda x:x>=20.8),B10=fr(u,lambda x:x>=24.5),
          G=mean(d['G']),H=mean(h),H50=pct(h,.5),H90=pct(h,.9),H99=pct(h,.99),Hge4=fr(h,lambda x:x>=4),Hge6=fr(h,lambda x:x>=6),TP=mean(d['TP']),Hnull=fr(d['Hnull'],bool),
          Ta=mean(d['Ta']),Ta10=pct(d['Ta'],.1),SST=mean(d['SST']),dT=mean(d['dT']),RH=mean(d['RH']),CC=mean(d['CC']),CC68=fr(d['CC'],lambda x:x>=75),CC28=fr(d['CC'],lambda x:x<=25),CL=mean(d['CL']),
          pr01=fr(d['pr01'],bool),pr2=fr(d['pr2'],bool),snow=fr(d['snowh'],bool),adv=fr(d['advfog'],bool),sat=fr(d['sat'],bool),smoke=fr(d['smoke'],bool),
          ppr_any=fr(d['ppr'],lambda x:x>0),ppr_mod=fr(d['ppr'],lambda x:x>=22.4),ppr_hvy=fr(d['ppr'],lambda x:x>=53.3),icmod=fr(d['icmod'],bool),icsev=fr(d['icsev'],bool),
          Dsect=[fr([D[i] for i in range(n) if mon[i]==mo and U[i]>=0.5],lambda x,k=k:((x+22.5)%360)//45==k) for k in range(8)],
          SS=[fr(h,lambda x,k=k:douglas(x)==k) for k in range(10)])
    R['mon']=mt
    # seasonal
    se={}
    for s in ['DJF','MAM','JJA','SON']:
        idx=[i for i in range(n) if SEAS[mon[i]]==s]
        u=[U[i] for i in idx]
        # anomaly autocorrelation (6-hourly subsample)
        mm={mo:mt[mo]['U'] for mo in mt}
        def acf(arr,lag,idx=idx,mmk='U'):
            pairs=[(arr[i]-mt[mon[i]][mmk],arr[i+lag]-mt[mon[i+lag]][mmk]) for i in idx[::3] if i+lag<n and SEAS[mon[i+lag]]==s and arr[i] is not None and arr[i+lag] is not None]
            if not pairs: return float('nan')
            x=[p[0] for p in pairs]; y=[p[1] for p in pairs]; mx=mean(x); my=mean(y)
            sx=math.sqrt(mean([(v-mx)**2 for v in x])); sy=math.sqrt(mean([(v-my)**2 for v in y]))
            return mean([(x[j]-mx)*(y[j]-my) for j in range(len(x))])/(sx*sy)
        ac={L:acf(U,L) for L in (1,3,6,12,24,48,72)}
        ach={L:acf(H,L,mmk='H') for L in (6,12,24,48,72)}
        # events
        def events(arr,thr):
            ev=[];run=0
            for i in idx:
                x=arr[i]
                if x is not None and x>=thr: run+=1
                else:
                    if run>0: ev.append(run)
                    run=0
            if run: ev.append(run)
            ev=[e for e in ev if e>=3]   # >=3 h
            yrs=10*len(set(mon[i] for i in idx))/12
            return dict(n_per_season=len(ev)/10, durmed=pct(ev,.5), dur90=pct(ev,.9), durmax=max(ev) if ev else 0)
        # MSLP synoptic: minima of 24h-mean P relative to 7-day running mean with depth >=4hPa
        # wind-wave lag correlation
        best=None
        for L in range(0,25,2):
            pairs=[(U[i],H[i+L]) for i in idx[::3] if i+L<n and U[i] is not None and H[i+L] is not None]
            x=[p[0] for p in pairs]; y=[p[1] for p in pairs]; mx=mean(x); my=mean(y)
            c=mean([(x[j]-mx)*(y[j]-my) for j in range(len(x))])/math.sqrt(mean([(v-mx)**2 for v in x])*mean([(v-my)**2 for v in y]))
            if best is None or c>best[1]: best=(L,c)
        # Hs vs U in bins (U>=? ) : median Hs for U bins
        bins={}
        for lo in (5,10,15,20):
            hs=[H[i] for i in idx if U[i] is not None and lo<=U[i]<lo+5 and H[i] is not None]
            bins[lo]=(pct(hs,.5),len(hs))
        # diurnal amplitude of U (local solar hour)
        lon=json.load(open(t+'_atm.json'))['longitude']
        dh=defaultdict(list)
        for i in idx:
            if U[i] is not None: dh[int((hr[i]+lon/15)%24)].append(U[i])
        dm={k:mean(v) for k,v in dh.items()}
        # dir-change frontal proxy not computed
        se[s]=dict(ac=ac,ach=ach,g7=events(U,13.9),g8=events(U,17.2),g10=events(U,24.5),h4=events(H,4.0),lag=best,hsU=bins,
                   diurn=(max(dm.values())-min(dm.values()), max(dm,key=dm.get), min(dm,key=dm.get)))
    # synoptic period from pressure: count minima of 24h smoothed P that are >=3 hPa below 5-day mean
    Ps=[None]*n
    for i in range(12,n-12):
        w=[P[j] for j in range(i-12,i+13) if P[j] is not None]; Ps[i]=mean(w)
    mins=defaultdict(int)
    for i in range(60,n-60):
        if Ps[i] is None: continue
        if all(Ps[j] is not None and Ps[i]<=Ps[j] for j in range(i-36,i+37)):
            ref=mean([P[j] for j in range(i-60,i+61)])
            if ref-Ps[i]>=3: mins[SEAS[mon[i]]]+=1
    for s in se: se[s]['troughs_per_season']=mins[s]/10; se[s]['interval_days']=(90.25/(mins[s]/10)) if mins[s] else None
    # 6-hourly Markov on wind classes
    cls=lambda u:0 if u<5.5 else (1 if u<10.8 else (2 if u<17.2 else 3))
    for s in se:
        Mk=[[0]*4 for _ in range(4)]
        for i in range(0,n-6,6):
            if SEAS[mon[i]]==s and U[i] is not None and U[i+6] is not None: Mk[cls(U[i])][cls(U[i+6])]+=1
        se[s]['markov']=[[round(100*c/max(1,sum(r)),1) for c in r] for r in Mk]
        se[s]['markov_n']=[sum(r) for r in Mk]
    R['sea']=se
    out[t]=R
    print(t,'ok',file=sys.stderr)
json.dump(out,open('stats_gib.json','w'),indent=0)
