"""Decompressore Microsoft EXEPACK -> MZ non compresso (con tabella di rilocazione ricostruita)."""
import struct, sys

def unpack(data):
    e = list(struct.unpack_from('<14H', data, 0))
    hdr = e[4] * 16
    img = data[hdr:]
    cs, ip = e[11], e[10]
    xh = cs * 16
    real_ip, real_cs, _mem, xsize, real_sp, real_ss, dest_len = struct.unpack_from('<7H', img, xh)
    if img[xh + 14:xh + 16] == b'RB':
        skip = 1; hlen = 16
    else:
        skip = struct.unpack_from('<H', img, xh + 14)[0]; hlen = 18
        assert img[xh + 16:xh + 18] == b'RB', 'firma RB non trovata'
    comp = bytearray(img[:xh])
    dst = bytearray(dest_len * 16)
    dst[:len(comp)] = comp
    si = len(comp)
    while si > 0 and comp[si - 1] == 0xff:
        si -= 1
    di = len(dst)
    while True:
        cmd = comp[si - 1]; n = comp[si - 3] | comp[si - 2] << 8; si -= 3
        if cmd & 0xfe == 0xb0:
            b = comp[si - 1]; si -= 1
            dst[di - n:di] = bytes([b]) * n; di -= n
        elif cmd & 0xfe == 0xb2:
            dst[di - n:di] = comp[si - n:si]; si -= n; di -= n
        else:
            raise ValueError('comando sconosciuto 0x%02x' % cmd)
        if cmd & 1:
            break
    # dst[:di] coincide con comp[:si] (dati non compressi in testa)
    dst[:si] = comp[:si]
    # tabella rilocazioni dopo il messaggio di errore nello stub
    msg = img.find(b'Packed file is corrupt', xh)
    p = msg + len(b'Packed file is corrupt')
    relocs = []
    for seg in range(16):
        cnt = struct.unpack_from('<H', img, p)[0]; p += 2
        for _ in range(cnt):
            relocs.append((struct.unpack_from('<H', img, p)[0], seg * 0x1000)); p += 2
    # nuovo MZ
    hdr_par = (0x1c + 4 * len(relocs) + 15) // 16
    total = hdr_par * 16 + len(dst)
    out = bytearray(hdr_par * 16)
    struct.pack_into('<14H', out, 0, 0x5a4d, total % 512, (total + 511) // 512, len(relocs), hdr_par,
                     e[5], e[6], real_ss, real_sp, 0, real_ip, real_cs, 0x1c, 0)
    for i, (o, s) in enumerate(relocs):
        struct.pack_into('<HH', out, 0x1c + 4 * i, o, s)
    return bytes(out + dst), len(relocs)

if __name__ == '__main__':
    out, n = unpack(open(sys.argv[1], 'rb').read())
    open(sys.argv[2], 'wb').write(out)
    print(f'{sys.argv[1]} -> {sys.argv[2]}: {len(out)} byte, {n} rilocazioni')
