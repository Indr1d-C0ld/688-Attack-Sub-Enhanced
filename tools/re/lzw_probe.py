import sys, struct
def bits_lsb(d, start):
    acc=0; n=0
    for b in d[start:]:
        acc |= b << n; n += 8
        while True:
            yield acc, n
            break
def lzw(d, start, msb, first_free, clear, eof, maxbits=12):
    # restituisce (n_codici, output)
    pos=start*8; total=len(d)*8; width=9; table=[bytes([i]) for i in range(256)]
    while len(table)<first_free: table.append(b'')
    out=bytearray(); prev=None; ncodes=0
    while pos+width<=total:
        if msb:
            v=0
            for i in range(width):
                bi=pos+i; v=(v<<1)|((d[bi>>3]>>(7-(bi&7)))&1)
        else:
            v=0
            for i in range(width):
                bi=pos+i; v|=((d[bi>>3]>>(bi&7))&1)<<i
        pos+=width; ncodes+=1
        if v==clear:
            table=table[:first_free]; width=9; prev=None; continue
        if v==eof: break
        if v<len(table) and (v>=256 or True) and (table[v] or v<256):
            e=table[v]
        elif v==len(table) and prev is not None:
            e=prev+prev[:1]
        else:
            return ncodes, out, 'bad code %d at %d'%(v,ncodes)
        out+=e
        if prev is not None and len(table)<(1<<maxbits):
            table.append(prev+e[:1])
        prev=e
        if len(table)+1>=(1<<width) and width<maxbits: width+=1
    return ncodes, out, 'ok'
d=open(sys.argv[1],'rb').read()
a,b=struct.unpack_from('<HH',d,0)
print('hdr',a,b,len(d))
for start in (4,8):
  for msb in (0,1):
    for ff,cl,eo in ((256,-1,-1),(257,256,-1),(258,256,257),(258,257,256)):
        n,o,s=lzw(d,start,msb,ff,cl,eo)
        print(start,msb,ff,cl,eo,'codes',n,'out',len(o),s, o[:16].hex())
