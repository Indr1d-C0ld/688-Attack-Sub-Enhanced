import json, math
T=['faroe','barents','gib_alboran','otranto','kunashir','kuril_pac','celtica','baltico']
def gumbel(x):
    n=len(x); m=sum(x)/n; s=math.sqrt(sum((v-m)**2 for v in x)/(n-1))
    b=s*math.sqrt(6)/math.pi; mu=m-0.5772*b
    return lambda T: mu-b*math.log(-math.log(1-1/T))
for t in T:
    a=json.load(open(t+'_atm.json'))['hourly']; mm=json.load(open(t+'_mar.json'))['hourly']
    yrs=sorted(set(s[:4] for s in a['time']))
    mx={k:[] for k in ('U','G','H')}
    for y in yrs:
        idx=[i for i,s in enumerate(a['time']) if s[:4]==y]
        mx['U'].append(max(a['wind_speed_10m'][i] for i in idx if a['wind_speed_10m'][i] is not None))
        mx['G'].append(max(a['wind_gusts_10m'][i] for i in idx if a['wind_gusts_10m'][i] is not None))
        mx['H'].append(max(mm['wave_height'][i] for i in idx if mm['wave_height'][i] is not None))
    # gust exceedance per season
    G=a['wind_gusts_10m']; mon=[int(s[5:7]) for s in a['time']]
    def fr(ms,thr):
        v=[G[i] for i in range(len(G)) if mon[i] in ms and G[i] is not None]; return 100*sum(1 for x in v if x>=thr)/len(v)
    line=[t]
    for k in ('U','G','H'):
        g=gumbel(mx[k]); line.append(f"{k}: max10a={max(mx[k]):.1f} medio_annuo={sum(mx[k])/10:.1f} R10={g(10):.1f} R50={g(50):.1f}")
    line.append('raffica>=24.5 DJF %.2f%% JJA %.2f%%; >=32.7 DJF %.3f%%'%(fr((12,1,2),24.5),fr((6,7,8),24.5),fr((12,1,2),32.7)))
    print(' | '.join(line))
