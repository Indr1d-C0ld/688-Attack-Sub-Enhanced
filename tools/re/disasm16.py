"""Disassemblato lineare di un intervallo di un segmento di GAME.EXE scompattato.
uso: disasm16.py GAME.EXE SEG INIZIO FINE (esadecimali; segmento 1000h all inizio dell immagine)"""
import struct, sys, capstone
d = open(sys.argv[1], 'rb').read()
img = d[struct.unpack_from('<H', d, 8)[0] * 16:]
seg = int(sys.argv[2], 16); a = int(sys.argv[3], 16); b = int(sys.argv[4], 16)
base = (seg - 0x1000) * 16
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_16)
off = a
while off < b:
    got = False
    for ins in md.disasm(img[base+off:base+off+16], off):
        print(f'{seg:04x}:{ins.address:04x}  {ins.bytes.hex():14s} {ins.mnemonic:6s} {ins.op_str}')
        off += ins.size; got = True
        break
    if not got:
        print(f'{seg:04x}:{off:04x}  {img[base+off]:02x}             db'); off += 1
