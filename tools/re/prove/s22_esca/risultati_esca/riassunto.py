import re,glob,statistics as st
for m in ['nascosto','scatto']:
    R=[]
    for s in range(1,13):
        l=[x for x in open(f'esca/{m}_{s}.txt') if x.startswith('seme')][0]
        d={k:int(v) for k,v in re.findall(r'(esito|liberi|fine5|fine15|fine30|vis|alfa30|scatti) (-?\d+)',l)}
        d['vict']=len(re.findall(r'[3-6]<-\{\}',l)); d['alfa']=len(re.findall(r'0<-\{\}',l))
        R.append(d)
    print(m,'liberi',[r['liberi'] for r in R],'media %.2f'%st.mean(r['liberi'] for r in R),'esito40',sum(r['esito']==40 for r in R))
    print('  victor liberi media %.2f, alfa libero %d'%(st.mean(r['vict'] for r in R),sum(r['alfa'] for r in R)), 'alfa30',sum(r['alfa30'] for r in R))
    for k in ['fine5','fine15','fine30','vis']:
        print('  ',k,[r[k] for r in R],'media %.2f'%st.mean(r[k] for r in R),'>=3:',sum(r[k]>=3 for r in R))
    print('   scatti',[r['scatti'] for r in R])
