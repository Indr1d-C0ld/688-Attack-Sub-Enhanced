"""Decompressore LZW di 688 Attack Sub (ricostruito da GAME.EXE, segmento 2283h).

Header: u16 numero_codici, u16 lunghezza_output; poi flusso di bit LSB-first.
Larghezza codice: il primo codice e' a 8 bit, poi 9..12 bit (cresce quando next >= limite).
Nessun codice di clear: il dizionario si reinizializza quando next raggiunge 0x1000.
"""
import struct

def decompress(d):
    count, outlen = struct.unpack_from('<HH', d, 0)
    pos = 4 * 8
    def read(width):
        nonlocal pos
        v = 0
        for i in range(width):
            v |= ((d[(pos + i) >> 3] >> ((pos + i) & 7)) & 1) << i
        pos += width
        return v
    def reset():
        return [None] * 0x1000, 0x100, 8, 0x100   # prefissi, next, larghezza, limite
    prefix, nxt, width, limit = reset()
    suffix = list(range(256)) + [0] * (0x1000 - 256)
    def expand(code):
        s = bytearray()
        while True:
            s.append(suffix[code])
            p = prefix[code] if code >= 256 else None
            if p is None: break
            code = p
        s.reverse()
        return s
    out = bytearray()
    old = read(width)
    first = expand(old); out += first; ch = first[0]
    count -= 1
    while count:
        if nxt == 0x1000:
            prefix, nxt, width, limit = reset()
        if nxt >= limit:
            width += 1; limit <<= 1
        code = read(width)
        if code != nxt:
            s = expand(code); out += s; ch_new = s[0]
            if nxt < 0x1000:
                prefix[nxt] = old; suffix[nxt] = ch_new; nxt += 1
            ch = ch_new
        else:
            if nxt < 0x1000:
                prefix[nxt] = old; suffix[nxt] = ch; nxt += 1
            s = expand(code); out += s; ch = s[0]
        old = code
        count -= 1
    return bytes(out), outlen

if __name__ == '__main__':
    import sys
    for f in sys.argv[1:]:
        o, n = decompress(open(f, 'rb').read())
        print(f, len(o), 'atteso', n, 'OK' if len(o) == n else 'DIFF', o[:16].hex(' '))
