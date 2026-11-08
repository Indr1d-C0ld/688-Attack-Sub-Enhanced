"""Lettore/estrattore degli archivi .MLB di 688 Attack Sub (Electronic Arts, 1989).

Formato (ricostruito):
  u16 count, u16 unknown
  count x { u32 offset, u32 size, char name[13] }   (nome 8.3 terminato da NUL, padding sporco)
"""
import struct, sys, os

def read_mlb(path):
    data = open(path, 'rb').read()
    count, unk = struct.unpack_from('<HH', data, 0)
    entries = []
    p = 4
    for _ in range(count):
        off, size = struct.unpack_from('<II', data, p)
        name = data[p+8:p+21].split(b'\0')[0].decode('ascii', 'replace')
        entries.append((name, off, size))
        p += 21
    return data, entries

if __name__ == '__main__':
    src, out = sys.argv[1], (sys.argv[2] if len(sys.argv) > 2 else None)
    data, entries = read_mlb(src)
    for name, off, size in entries:
        print(f'{name:14s} off=0x{off:06x} size={size:7d} head={data[off:off+12].hex(" ")}')
        if out:
            os.makedirs(out, exist_ok=True)
            open(os.path.join(out, name), 'wb').write(data[off:off+size])
