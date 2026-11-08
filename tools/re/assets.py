"""Estrazione asset grafici/audio di 688 Attack Sub.

Formati ricostruiti:
  *.PAK  : 64000 byte, bitmap 320x200 8bpp grezza (mode 13h)
  *.PAL  : 768 byte, 256 x RGB a 8 bit (non 6 bit VGA)
  *.ENC  : u16 larghezza, u16 altezza-1, poi RLE su byte (2 pixel/byte, nibble alto = sinistra),
           righe memorizzate dal basso verso l'alto. I nibble sono indici relativi a un banco
           di palette: 0 (EGA, interfaccia) oppure 144 (rampa di 16 grigi, foto da periscopio).
           ctrl & 0x80 -> ripeti il byte seguente (ctrl&0x7f)+3 volte
           altrimenti  -> copia ctrl+1 byte letterali
  *.LZW / *.LZA : LZW (vedi lzw.py); il risultato di *.LZW e' un'immagine in formato ENC.
           SURFA..SURFG = mappe batimetriche dei 7 teatri (nibble = livello di profondita' 0..15)
  *.T    : PCM 4 bit senza segno, 2 campioni/byte (ipotesi: nibble alto prima)
"""
import os, sys, glob, struct, wave
from PIL import Image
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from lzw import decompress

def load_pal(path):
    d = open(path, 'rb').read()
    return list(d[:768])

def pak_to_png(pak, pal, out):
    img = Image.frombytes('P', (320, 200), open(pak, 'rb').read())
    img.putpalette(pal)
    img.save(out)

def decode_enc(d):
    w, h1 = struct.unpack_from('<HH', d, 0)
    h = h1 + 1
    bpr = (w + 1) // 2
    need = bpr * h
    out = bytearray(); p = 4
    while p < len(d) and len(out) < need:
        c = d[p]; p += 1
        if c & 0x80:
            out += bytes([d[p]]) * ((c & 0x7f) + 3); p += 1
        else:
            out += d[p:p + c + 1]; p += c + 1
    if len(out) < need or w > 2000 or h > 2000:
        return None
    px = bytearray(w * h)
    for y in range(h):
        for x in range(w):
            b = out[y * bpr + x // 2]
            px[y * w + x] = (b >> 4) if x % 2 == 0 else (b & 15)
    return w, h, bytes(px)

# prefissi degli sprite fotografici (banco grigi 144); elenco preso dalle stringhe di GAME.EXE
GRAY_PREFIXES = ('FOREST', 'MERCH', 'CARGO', 'SFX2', 'KARA', 'KIROV', 'KRIVAK', 'NUCHKA', 'SLAVA',
                 'NIMITZ', 'IOWA', 'KIDD', 'OHP', 'SAC', 'TANKER', 'SEAHAWK', 'MISSILE', 'CREW', 'SMK')

def enc_to_png(path, pal, out, scale=1):
    r = decode_enc(open(path, 'rb').read())
    if not r: return False
    w, h, px = r
    name = os.path.basename(path).upper()
    bank = 144 if name.startswith(GRAY_PREFIXES) else 0
    img = Image.frombytes('P', (w, h), bytes(p + bank for p in px))
    img.putpalette(pal)
    img = img.transpose(Image.FLIP_TOP_BOTTOM)
    if scale != 1:
        img = img.resize((w * scale, h * scale), Image.NEAREST)
    if bank and not name.startswith('CREW'):
        img.save(out, transparency=bank + 15)  # nibble 15 = trasparente sulle sagome
    else:
        img.save(out)
    return True

def t_to_wav(path, out, rate=8000):
    d = open(path, 'rb').read()
    s = bytearray()
    for b in d:
        s.append((b >> 4) * 17); s.append((b & 15) * 17)
    with wave.open(out, 'wb') as w:
        w.setnchannels(1); w.setsampwidth(1); w.setframerate(rate); w.writeframes(bytes(s))

if __name__ == '__main__':
    root = sys.argv[1]
    game = os.path.join(root, 'original-game', '688 attack sub')
    raw = os.path.join(root, 'extracted', 'raw')
    outd = os.path.join(root, 'extracted', 'png'); os.makedirs(outd, exist_ok=True)
    snd = os.path.join(root, 'extracted', 'wav'); os.makedirs(snd, exist_ok=True)
    for n in ('688', 'ALFA', 'TITLE'):
        pak_to_png(f'{game}/{n}.PAK', load_pal(f'{game}/{n}.PAL'), f'{outd}/{n}.png')
    day = load_pal(f'{raw}/DISK1/DAY.PAL')
    night = load_pal(f'{raw}/DISK1/NIGHT.PAL')
    bad = []
    for f in sorted(glob.glob(f'{raw}/*/*.ENC')):
        b = os.path.splitext(os.path.basename(f))[0]
        if not enc_to_png(f, day, f'{outd}/{b}.png'): bad.append(b)
    # schermate LZW (centrali di comando, schermate finali) e mappe batimetriche
    depth_pal = []
    for i in range(16):   # rampa neutra: il significato esatto dei livelli va confermato dal codice
        t = i / 15
        depth_pal += [int(10 + 220 * t), int(20 + 220 * t), int(60 + 190 * t)]
    bathy = os.path.join(root, 'extracted', 'bathymetry'); os.makedirs(bathy, exist_ok=True)
    for f in sorted(glob.glob(f'{raw}/*/*.LZW')):
        b = os.path.splitext(os.path.basename(f))[0]
        r = decode_enc(decompress(open(f, 'rb').read())[0])
        if not r: continue
        w, h, px = r
        img = Image.frombytes('P', (w, h), px).transpose(Image.FLIP_TOP_BOTTOM)
        if b.startswith('SURF'):
            open(f'{bathy}/{b}_{w}x{h}.u8', 'wb').write(img.tobytes())
            img.putpalette(day); img.save(f'{outd}/{b}_originale.png')
            img.putpalette(depth_pal + [0] * (768 - 48))
            img.save(f'{bathy}/{b}.png')
        else:
            img.putpalette(day); img.save(f'{outd}/{b}.png')
    for f in sorted(glob.glob(f'{raw}/*/*.T')):
        t_to_wav(f, f'{snd}/{os.path.splitext(os.path.basename(f))[0]}.wav')
    print('ENC non decodificati:', bad)
    print('PNG:', len(os.listdir(outd)), 'WAV:', len(os.listdir(snd)))
