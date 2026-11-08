"""Listato dei gestori degli opcode della VM degli scenari (.TOK) in GAME.EXE decompresso."""
import struct, sys, capstone
d = open(sys.argv[1], 'rb').read()
img = d[struct.unpack_from('<H', d, 8)[0] * 16:]
DS = 0x13060; SEG = (0x2011 - 0x1000) * 16
tab = DS + 0x6505
handlers = []
for i in range(256):
    v = struct.unpack_from('<H', img, tab + 2 * i)[0]
    if not (0x1300 <= v < 0x3000): break
    handlers.append(v)
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_16)
print(f'; {len(handlers)} opcode (1..{len(handlers)}), tabella a DS:6505, codice nel segmento 2011h')
for op, h in enumerate(handlers, 1):
    print(f'\n;===== opcode 0x{op:02x} -> 2011:{h:04x}')
    n = 0
    for ins in md.disasm(img[SEG + h:SEG + h + 0x200], h):
        print(f'  {ins.address:04x}  {ins.mnemonic:6s} {ins.op_str}')
        n += 1
        if ins.mnemonic in ('ret', 'retf', 'jmp') or n > 60: break
