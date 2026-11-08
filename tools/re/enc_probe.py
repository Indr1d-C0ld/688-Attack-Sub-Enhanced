import sys, glob, os
def dec(d, mode):
    w = d[0]|d[1]<<8; h = d[2]|d[3]<<8; p=4; out=bytearray()
    while p < len(d):
        c = d[p]; p+=1
        if c & 0x80:
            n = (c & 0x7f) + mode[0]
            if p>=len(d): break
            out += bytes([d[p]])*n; p+=1
        else:
            n = c + mode[1]
            out += d[p:p+n]; p+=n
    return w,h,len(out)
for f in sorted(glob.glob(sys.argv[1]+'/*.ENC'))[:40]:
    d=open(f,'rb').read()
    res=[]
    for m in [(1,1),(2,1),(3,1),(1,0)]:
        w,h,n=dec(d,m); res.append(n)
    print(f'{os.path.basename(f):14s} {w:4d}x{h:<4d} need4bpp={((w+1)//2)*h:6d} need8={w*h:6d}', res)
