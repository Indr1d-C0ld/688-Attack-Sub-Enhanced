import sys, capstone
d=open(sys.argv[1],'rb').read()
hdr=int.from_bytes(d[8:10],'little')*16
img=d[hdr:]
targets={int(x,16) for x in sys.argv[2:]}
md=capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_16)
# scansione lineare grezza del segmento di codice (fino all'inizio dei dati)
end=0x13050
for off in range(0,end):
    for ins in md.disasm(img[off:off+8], off):
        for t in targets:
            if f'0x{t:x}' in ins.op_str.split(', ')[-1:][0] if ins.op_str else False:
                print(hex(off), ins.mnemonic, ins.op_str)
        break
