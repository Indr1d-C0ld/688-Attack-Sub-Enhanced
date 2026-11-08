# solo i sottomarini dell'IA: per ogni gruppo di copie, i dati su bersagli ostili e i contatti solidi ostili in corso (dagli eventi), con o senza posizione. Solo lettura
import subprocess,re,collections,pathlib,glob
R='/home/randolph/Claude/melchior-688'; B=R+'/engine/build_radio'; P=R+'/engine/build_portata/sonda_tracce'; M=B+'/fisso/sonda_m4'
giochi=[('misura',f'{m}_{s}') for m in 'I02 I05 S08 S11'.split() for s in (1,2)]+[('misura_lunghe',f'{m}_{s}') for m in 'I06 I07 S03 S07 S15'.split() for s in (1,2)]
tot=collections.Counter()
for d,n in giochi:
    m=n.split('_')[0]; f=glob.glob(f'{R}/data/missioni/libreria/LIB_{m}_*.lua')[0]; t=pathlib.Path(f).read_text()
    un={}
    for x in re.finditer(r'\{ id = (\d+), classe = "(\w+)", categoria = "(\w+)", parte = "(\w+)", ruolo = "[^"]*", flag = (\w+), giocatore = (\w+), non_combattente = (\w+)',t):
        un[x[1]]=dict(cl=x[2],cat=x[3],parte=x[4],flag=x[5],gioc=x[6]=='true',nc=x[7]=='true')
    sit=re.findall(r'situazione = "(\w+)", paranoia',t); eq=dict(re.findall(r'unita = (\d+), missione = "\w+", situazione = "\w+", bersagli = "?\w*"?, equipaggio = "(\w+)"',t))
    txt=pathlib.Path(f'{B}/{d}/{n}.txt').read_text(errors='replace').splitlines()
    cop=collections.defaultdict(lambda:collections.defaultdict(list)); us=collections.defaultdict(list); sal=collections.Counter(); nav=0
    for l in txt:
        if l.startswith('QUADRO'):
            k=dict(x.split('=') for x in l.split()[2:] if '=' in x)
            if l.startswith('QUADRO copia'):
                if un.get(k['id'],{}).get('cat')=='sottomarino': cop[k['id']][int(k['t'])//20*20].append(k['su'])
                else: nav+=1
            elif l.startswith('QUADRO usato'): us[k['id']].append((k['t'],k['su']))
            elif l.startswith('QUADRO salita'): sal[k['id']]+=1
    tr=f'{B}/{d}/{n}.m688tr'
    print(f"== {n} sit={sit} equip={eq} copie su navi/aerei={nav}")
    for i in sorted(set(cop)|set(sal),key=int):
        pu=un.get(i,{}).get('parte')
        o=subprocess.run([P,tr,i],capture_output=True,text=True).stdout.splitlines()[1:]
        pos={f.split()[0]:float(f.split()[5]) for f in o if len(f.split())>=7}
        ev=subprocess.run([M,tr,i],capture_output=True,text=True).stdout.splitlines()
        inter=collections.defaultdict(list)
        for l in ev:
            x=re.match(r'\s+t=\s*([\d.]+) (contatto_solido|contatto_perso) altra=(-?\d+)',l)
            if x:
                if x[2]=='contatto_solido': inter[x[3]].append([float(x[1]),1e9])
                elif inter[x[3]] and inter[x[3]][-1][1]==1e9: inter[x[3]][-1][1]=float(x[1])
        ost=lambda a: a in un and un[a]['parte']!=pu and un[a]['cat']!='aereo'
        print(f" id={i} {un.get(i,{}).get('cl')} {pu} salite={sal[i]} copie={sum(len(v) for v in cop[i].values())} usi={us[i]}")
        for t0 in sorted(cop[i]):
            su=sorted(set(cop[i][t0]),key=int)
            dati_ost=[a for a in su if ost(a) and not un[a]['nc']]
            att=[a for a,v in inter.items() if ost(a) and any(x<=t0+25 and t0+5<=y for x,y in v)]
            cp=[a for a in att if 0<=pos.get(a,-1)<=t0+25]
            cat='posizione' if cp else 'rilevamento' if att else 'nessuno'
            if dati_ost: tot[cat]+=1
            print(f"   t={t0:6} dati={su[:7]} ostili={dati_ost[:6]} solidi_ostili={sorted(att,key=int)[:6]} con_pos={cp[:4]} -> {cat if dati_ost else '(dati non ostili)'}")
print(dict(tot))
