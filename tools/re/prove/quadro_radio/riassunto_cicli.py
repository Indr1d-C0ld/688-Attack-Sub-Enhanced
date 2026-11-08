# per partita e battello: cicli guardati e, per ciclo, se almeno una riga e' libera / bloccata dal solo "rimanda" col solo rilevamento (vicino assente) / altro. Solo lettura
import pathlib,collections,sys
B='/home/randolph/Claude/melchior-688/engine/build_radio'
G=[('misura',n) for n in 'm4a_1 m4a_2 m4a_3 m4a_5 m4c_2 m4c_6 m4c_7 m4c_10 s5_1 s5_3 s5_7 I02_1 I02_2 I05_1 I05_2 S08_1 S08_2 S11_1 S11_2'.split()]+[('misura_lunghe',f'{m}_{s}') for m in 'I06 I07 S03 S07 S15'.split() for s in (1,2)]
AL='script affonda difesa reazione tma aggancio missile pedina vetrino evasione banchisa'.split()
for d,n in G:
    per=collections.defaultdict(lambda:collections.defaultdict(list)); ch=collections.Counter(); sal=collections.Counter(); rv=[]
    for l in pathlib.Path(f'{B}/{d}/{n}.txt').read_text(errors='replace').splitlines():
        if l.startswith('QUADRO finestra'):
            k=dict(x.split('=') for x in l.split()[2:]); per[k['id']][k['prossimo']].append(k)
            if k['rimanda']=='1': rv.append(float(k['vicino']))
        elif l.startswith('QUADRO chiusa'):
            k=dict(x.split('=') for x in l.split()[2:] if '=' in x)
            for m in ('script','occupato','rimanda','quota_altri','vel_altri'):
                if k.get(m)=='1': ch[m]+=1
        elif l.startswith('QUADRO salita'): sal[l.split()[3]]+=1
    r=[]
    for i,cc in sorted(per.items(),key=lambda x:int(x[0])):
        c=collections.Counter()
        for p,v in cc.items():
            lib=[k for k in v if not any(k[a]=='1' for a in AL) and k['tentato']=='0']
            if any(k['rimanda']=='0' for k in lib): c['libero']+=1
            elif any(float(k['vicino'])>1e17 for k in lib): c['solo_rilev']+=1
            elif lib: c['rimanda_pos']+=1
            else:
                c['bloccato']+=1
                ag=sum(k['aggancio']=='1' for k in v)
                if ag==len(v): c['_aggancio_sempre']+=1
        r.append(f"id{i}:{dict(c)}")
    rimp=sum(x>1e17 for x in rv)
    print(f"{n:7} {' '.join(r)} | rimanda righe {len(rv)} senza_pos {rimp} oltre15km {sum(15000<x<1e17 for x in rv)} | salite {sum(sal.values())} chiuse {dict(ch)}")
