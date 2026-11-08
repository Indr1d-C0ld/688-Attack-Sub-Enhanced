# Per il capitolo 9 di proposta.md: dalle tracce gia' giocate, quanto tempo ogni sottomarino dell'IA starebbe nei limiti del filo e dell'ELF, e i suoi rapporti di oggi. Solo lettura
import subprocess,re,glob,pathlib,collections,os
R='/home/randolph/Claude/melchior-688'; B=R+'/engine/build_radio'; F=B+'/fisso'
os.environ['LD_LIBRARY_PATH']=R+'/tools/deps/sysroot/usr/lib/x86_64-linux-gnu'
G=[('misura',f'{m}_{s}') for m in 'I02 I05 S08 S11'.split() for s in (1,2)]+[('misura_lunghe',f'{m}_{s}') for m in 'I06 I07 S03 S07 S15'.split() for s in (1,2)]
G+=[('misura',n) for n in 'm4a_1 m4a_2 m4a_3 m4a_5 m4c_2 m4c_6 m4c_7 m4c_10 s5_1 s5_3 s5_7'.split()]
for d,n in G:
    if n[0]=='m': subs={'6':('victor3','urss')}
    elif n[0]=='s' and n[1]=='5': subs={'12':('victor3','urss')}
    else:
        t=pathlib.Path(glob.glob(f"{R}/data/missioni/libreria/LIB_{n.split('_')[0]}_*.lua")[0]).read_text()
        subs={x[1]:(x[2],x[3]) for x in re.finditer(r'\{ id = (\d+), classe = "(\w+)", categoria = "sottomarino", parte = "(\w+)", ruolo = "[^"]*", flag = \w+, giocatore = false',t) if x[3]!='neutrale'}
    tr=f'{B}/{d}/{n}.m688tr'
    for i,(cl,pa) in sorted(subs.items(),key=lambda x:int(x[0])):
        o=subprocess.run([F+'/sonda_salite',tr,i,'0','20000','30'],capture_output=True,text=True).stdout
        c=collections.Counter()
        for x in re.finditer(r't=\s*(\d+) q=\s*([\d.-]+) rotta=\s*([\d.-]+) vel=\s*([\d.-]+) alberi=(\d+)',o):
            q,v=float(x[2]),float(x[4]); c['n']+=1
            if q<=120 and v<=8: c['f8']+=1
            if q<=120 and v<=12: c['f12']+=1
            if q<=300 and v<=15: c['e']+=1
            if q<=7.1: c['m']+=1
            if q<=20: c['q20']+=1
            if v>20: c['v20']+=1
            if q>120: c['prof']+=1
        m=subprocess.run([F+'/sonda_s5',tr,tr,i],capture_output=True,text=True).stdout
        msg=re.search(r"msg_v[:=]\s*(\d+)",m)
        N=max(c['n'],1)
        print(f"{n:7} id{i:>2} {cl:11} {pa:4} camp {c['n']:4} | filo<=8kn {100*c['f8']//N:3}% <=12kn {100*c['f12']//N:3}% | elf {100*c['e']//N:3}% | >120m {100*c['prof']//N:3}% | albero {100*c['m']//N:3}% <=20m {100*c['q20']//N:3}% | >20kn {c['v20']*30:5}s | rapporti {msg[1] if msg else '-'}")
