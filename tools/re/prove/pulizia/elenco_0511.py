# Pulizia del 05/11/2026 (gruppo A, approvato dall'utente): elenca binari ELF e oggetti di compilazione in tools/re/prove,
# fuori dalle cartelle ancora in uso. Risultati, registri e sorgenti restano. Scrive l'elenco; non cancella nulla.
import os,sys,collections,time
root='tools/re/prove'; KEEP={'verifica','s22_esca','difese_navi','quadro_radio','pulizia'}
ogg=('.o','.a','.gch','.pch','.d')
out=open('tools/re/prove/pulizia/elenco_0511.txt','w'); tot=0; nomi=collections.Counter(); ext=collections.Counter(); rec=[]
for top in sorted(os.listdir(root)):
    if top in KEEP or not os.path.isdir(os.path.join(root,top)): continue
    for d,ds,fs in os.walk(os.path.join(root,top)):
        for f in fs:
            p=os.path.join(d,f)
            if os.path.islink(p) or not os.path.isfile(p): continue
            st=os.lstat(p); elf=False
            with open(p,'rb') as h: elf=h.read(4)==b'\x7fELF'
            if not (elf or f.endswith(ogg) or '/CMakeFiles/' in p): continue
            out.write(p+'\n'); tot+=st.st_blocks*512
            if elf: nomi[f]+=1
            ext[os.path.splitext(f)[1]]+=1
            if time.time()-st.st_mtime<86400: rec.append(p)
print(f'{tot/1024**3:.1f} GB'); print('estensioni',ext.most_common(10)); print('ELF',nomi.most_common(30)); print('ultime 24 ore',rec[:15],len(rec))
