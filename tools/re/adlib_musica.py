"""Musica dei titoli di 688 Attack Sub per AdLib: sequencer di CALLS.EXE emulato e registrato in VGM.

CALLS.EXE suona la musica dei titoli con tre uscite, scelte da chi lo lancia: 0 = chip del Tandy/PCjr
(porta C0h, 1000:0034), 1 = Covox Speech Thing (campioni sulla porta della stampante, 1000:05FC),
2 = AdLib (porta 388h, 1000:0868). Il segmento dati e' DS = 00D3h (offset 0D30h nell'immagine).

AdLib: il timer 0 e' riprogrammato con divisore 4000h (1193182 / 16384 = 72,8 Hz); il gestore 1000:0D12
chiama a ogni tick il sequencer 1000:08DF, che fa avanzare 9 voci e alla fine scrive il registro BD
(ritmo). Dati: DS:0ADC tempo, DS:0ADE 9 puntatori alle liste di pattern. Lista = voci da 3 byte
[trasposizione][u16 pattern], chiusa da 80h 80h (si ricomincia). Nel pattern:
  00-7F nota (+ trasposizione, frequenza da DS:0821; 60h = pausa)   80h|d durata d (89h-FEh, x tempo)
  81 x glissando (x con segno, sommato a ogni tick)    82 n d vibrato (tabella DS:08E3[n], ampiezza 1/d)
  83 vibrato spento    84 fine    85 n strumento melodico (DS:0A28[n], 9 byte: 20/40/60/80 del modulatore
  e del portante, C0)    86 x rilascio (key off quando mancano x tick)    87 n grancassa sul canale 6
  (DS:0AAC[n])    88 n percussione a un operatore (DS:0AC7[n], 4 byte + offset dell'operatore)
  FF pattern successivo.
All'avvio (1000:0C8C) azzera i 102 registri elencati in DS:07A9 e porta a 3Fh (silenzio) i 18 livelli
d'uscita elencati in DS:080F.

Uso:  adlib_musica.py <radice>  ->  extracted/musica/titolo_adlib.vgm, .mid (note) e .wav (render con tools/ymfm/vgmrender)
"""
import os, sys, struct, subprocess

DS = 0xd30
PIT = 1193182
DIVISORE = 0x4000
VGM_HZ = 44100
OPL2_CLOCK = 3579545

class Immagine:
    def __init__(self, root):
        d = open(f'{root}/re/unpacked/CALLS.EXE', 'rb').read()
        self.m = bytearray(d[struct.unpack_from('<H', d, 8)[0] * 16:])
    def b(self, o): return self.m[DS + o]
    def w(self, o): return struct.unpack_from('<H', self.m, DS + o)[0]
    def sb(self, o): v = self.b(o); return v - 256 if v > 127 else v

class Voce:
    pass

class Sequencer:
    def __init__(self, im):
        self.im = im; self.scritture = []; self.bd = im.b(0xa27); self.fine = False
        self.tempo = im.b(0xadc); self.note = []; self.voci = []; self.giri = [None] * 9; self.tick = 0
        for c in range(9):
            v = Voce(); lista = im.w(0xade + 2 * c)
            v.inizio = v.voce_lista = lista; v.trasp = im.b(lista); v.pc = im.w(lista + 1)
            v.cont = 1; v.vib = 0; v.freq = v.base = v.blocco = 0; v.tab = v.tabpos = 0; v.rilascio = 0
            v.dur = 0; v.slide = 0; v.slide_on = 0; v.keyon = 0; v.modo = 0
            self.voci.append(v)
        self.azzera()

    def out(self, reg, val): self.scritture.append((reg & 0xff, val & 0xff))

    def azzera(self):                                   # 1000:0C8C
        for i in range(0x65, -1, -1): self.out(self.im.b(0x7a9 + i), 0)
        for i in range(0x11, -1, -1): self.out(self.im.b(0x80f + i), 0x3f)

    def strumento(self, ch, p):                         # 1000:0B62: 9 byte
        im = self.im; mod = im.b(0xa06 + ch); car = im.b(0xa0f + ch)
        for k, r in enumerate((0x20, 0x40, 0x60, 0x80)): self.out(mod + r, im.b(p + k))
        for k, r in enumerate((0x20, 0x40, 0x60, 0x80)): self.out(car + r, im.b(p + 4 + k))
        self.out(0xc0 + ch, im.b(p + 8))

    def operatore(self, op, p):                         # 1000:0C1F: 4 byte su un operatore
        for k, r in enumerate((0x20, 0x40, 0x60, 0x80)): self.out(op + r, self.im.b(p + k))

    def scrivi_freq(self, ch, v):
        if v.modo == 0:                                 # 1000:0AAB, canale melodico
            self.out(0xa0 + ch, v.freq & 0xff); self.out(0xb0 + ch, (v.freq >> 8) | v.keyon)
        elif v.modo == 1:                               # 1000:0AD5, grancassa sul canale 6
            self.out(0xa6, v.freq & 0xff); self.out(0xb6, v.freq >> 8)
            self.bd = (self.bd | 0x10) if v.keyon else (self.bd & 0xef)
        else:                                           # 1000:0B0D, percussioni a un operatore
            i = v.modo - 0x11; c = self.im.b(0xa1d + i); bit = self.im.b(0xa18 + i)
            self.out(0xa0 + c, v.freq & 0xff); self.out(0xb0 + c, v.freq >> 8)
            self.bd = (self.bd | bit) if v.keyon else (self.bd & ~bit & 0xff)

    def passo(self):                                    # 1000:08DF
        if self.fine: return
        im = self.im
        for ch, v in enumerate(self.voci):
            v.cont = (v.cont - 1) & 0xffff
            if v.cont == 0:
                v.slide_on = 0; pc = v.pc
                while True:
                    a = im.b(pc); pc += 1
                    if not a & 0x80: break
                    if a == 0x85:
                        n = im.b(pc); pc += 1; self.strumento(ch, im.w(0xa28 + 2 * n)); v.modo = 0
                    elif a == 0x87:
                        n = im.b(pc); pc += 1; self.strumento(6, im.w(0xaac + 2 * n)); v.modo = 1
                    elif a == 0x88:
                        n = im.b(pc); pc += 1; p = im.w(0xac7 + 2 * n); v.modo = im.b(p + 4); self.operatore(v.modo, p)
                    elif a == 0x81: v.slide = im.sb(pc); pc += 1; v.slide_on = 1
                    elif a == 0x82: v.tab = im.w(0x8e3 + 2 * im.b(pc)); v.vib = im.b(pc + 1); pc += 2
                    elif a == 0x83: v.vib = 0
                    elif a == 0x84: self.fine = True; self.azzera(); self.azzera(); return
                    elif a == 0x86: v.rilascio = im.b(pc); pc += 1
                    elif a == 0xff:
                        e = v.voce_lista + 3
                        if im.w(e) == 0x8080:
                            e = v.inizio
                            if self.giri[ch] is None: self.giri[ch] = self.tick
                        v.trasp = im.b(e); pc = im.w(e + 1); v.voce_lista = e
                    else: v.dur = a & 0x3f
                v.pc = pc
                self.note.append((self.tick, ch, None if a == 0x60 else (a + v.trasp) & 0xff, v.modo, v.cont_prec if hasattr(v, 'cont_prec') else 0))
                if a == 0x60: v.keyon = 0
                else:
                    f = im.w(0x821 + 2 * ((a + v.trasp) & 0xff))
                    v.freq = v.base = f; v.blocco = f & 0x1c00; v.keyon = 0x20
                self.scrivi_freq(ch, v)
                v.tabpos = v.tab
                v.cont = (v.dur * self.tempo) & 0xffff
            else:                                       # 1000:0A3B
                if v.cont < v.rilascio and v.keyon: v.keyon = 0; self.note.append((self.tick, ch, 'off', v.modo, 0))
                if v.slide_on:
                    v.freq = (((v.freq & 0x3ff) + v.slide) & 0xffff) | v.blocco; v.base = v.freq
                if v.vib:
                    base = v.base & 0x3ff; q = (base // v.vib) & 0xff
                    q = q - 256 if q > 127 else q
                    t = im.sb(v.tabpos)
                    if im.b(v.tabpos) == 0x80: v.tabpos = v.tab; t = im.sb(v.tabpos)
                    v.tabpos += 1
                    v.freq = ((q * t + base) & 0xffff) | v.blocco
                self.scrivi_freq(ch, v)
        self.out(0xbd, self.bd)
        self.tick += 1

def vgm(scritture_per_tick, giro_tick):
    """VGM 1.51 per YM3812: 0x5A reg val, attese 0x61 in campioni a 44,1 kHz, punto di ripetizione."""
    dati = bytearray(); campioni = 0; resto = 0.0; ciclo_off = ciclo_campioni = None
    passo = VGM_HZ * DIVISORE / PIT
    for t, ww in enumerate(scritture_per_tick):
        if giro_tick and t == giro_tick: ciclo_off = len(dati); ciclo_inizio = campioni
        for r, v in ww: dati += bytes((0x5a, r, v))
        resto += passo; n = int(resto); resto -= n; campioni += n
        while n > 0:
            k = min(n, 65535); dati += bytes((0x61,)) + struct.pack('<H', k); n -= k
    dati += b'\x66'
    h = bytearray(0x100)
    h[0:4] = b'Vgm '
    struct.pack_into('<I', h, 0x04, 0x100 + len(dati) - 4)
    struct.pack_into('<I', h, 0x08, 0x151)
    struct.pack_into('<I', h, 0x18, campioni)
    struct.pack_into('<I', h, 0x34, 0x100 - 0x34)
    struct.pack_into('<I', h, 0x50, OPL2_CLOCK)
    return bytes(h + dati), campioni

def midi(note, fine_tick):
    """MIDI di tipo 1: una traccia per voce (canali 1-9; le percussioni sul canale 10, note GM).
    Nota del sequencer n -> MIDI n + 12 (la nota 48 della tabella e' il Do centrale, 261 Hz).
    Tempo: 1 tick del sequencer = 1 tick MIDI, 72,8 tick al secondo."""
    GM = {1: 36, 0x10: 42, 0x11: 42, 0x12: 49, 0x13: 45, 0x14: 38, 0x15: 42}
    usec = round(1e6 * DIVISORE / PIT * 96)        # 96 tick per semiminima
    def vlq(n):
        b = [n & 0x7f]
        while n > 0x7f: n >>= 7; b.append(0x80 | (n & 0x7f))
        return bytes(reversed(b))
    tracce = [b'\x00\xff\x51\x03' + usec.to_bytes(3, 'big') + b'\x00\xff\x2f\x00']
    for ch in range(9):
        ev = []; suona = None
        for t, c, n, modo, _ in note:
            if c != ch: continue
            if suona is not None: ev.append((t, 0x80, suona)); suona = None
            if n is None or n == 'off': continue
            canale = 9 if modo else ch
            k = GM.get(modo, 38) if modo else n + 12
            ev.append((t, 0x90 | canale, k)); suona = (k, canale)
        if suona is not None: ev.append((fine_tick, 0x80, suona))
        d = bytearray(); ora = 0
        for t, tipo, k in ev:
            if tipo == 0x80: k, canale = k; st = 0x80 | canale; vel = 0
            else: st = tipo; vel = 100
            d += vlq(t - ora) + bytes((st, k & 0x7f, vel)); ora = t
        d += b'\x00\xff\x2f\x00'; tracce.append(bytes(d))
    out = b'MThd' + struct.pack('>IHHH', 6, 1, len(tracce), 96)
    for t in tracce: out += b'MTrk' + struct.pack('>I', len(t)) + t
    return out

if __name__ == '__main__':
    root = sys.argv[1]; im = Immagine(root); sq = Sequencer(im)
    dst = f'{root}/extracted/musica'; os.makedirs(dst, exist_ok=True)
    per_tick = [list(sq.scritture)]; sq.scritture.clear()
    while not sq.fine and sq.tick < 72 * 60 * 10 and None in sq.giri:
        sq.passo(); per_tick.append(list(sq.scritture)); sq.scritture.clear()
    giri = sq.giri
    durata = len(per_tick)
    dati, campioni = vgm(per_tick, None)
    open(f'{dst}/titolo_adlib.vgm', 'wb').write(dati)
    open(f'{dst}/titolo_adlib.mid', 'wb').write(midi(sq.note, durata))
    print(f"tempo {sq.tempo}  tick {durata} ({durata * DIVISORE / PIT:.1f} s)  fine {'84h' if sq.fine else 'ripetizione'}  "
          f"primo giro per voce {giri}  scritture {sum(len(x) for x in per_tick)}")
    r = f'{root}/tools/ymfm/vgmrender'
    if os.path.exists(r):
        subprocess.run([r, f'{dst}/titolo_adlib.vgm', '-o', f'{dst}/titolo_adlib.wav', '-r', '44100'], check=True,
                       stdout=subprocess.DEVNULL)
        # l'OPL2 e' monofonico: vgmrender scrive due canali uguali, si tiene un canale solo
        import wave
        with wave.open(f'{dst}/titolo_adlib.wav') as w: fr = w.readframes(w.getnframes()); hz = w.getframerate()
        mono = b''.join(fr[i:i + 2] for i in range(0, len(fr), 4))
        with wave.open(f'{dst}/titolo_adlib.wav', 'wb') as w:
            w.setnchannels(1); w.setsampwidth(2); w.setframerate(hz); w.writeframes(mono)
        print('wav', os.path.getsize(f'{dst}/titolo_adlib.wav'))
