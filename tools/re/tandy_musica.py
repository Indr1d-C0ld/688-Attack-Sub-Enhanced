"""Musica dei titoli nella versione Tandy 1000 / PCjr (CALLS.EXE, 1000:0034): emulazione di sequencer e chip.

Chip: SN76496 (Texas Instruments, porta C0h), 3 toni a onda quadra + rumore, clock 3,579545 MHz,
frequenza = clock / (32 N), volume = attenuazione a passi di 2 dB (15 = spento).
Sequencer (1000:012D), un passo a ogni ritorno verticale del video (circa 60 Hz su Tandy 1000).
Dati: DS:03C2 tempo, DS:03C4 lista della voce campionata, DS:03C6 liste delle 3 voci.
- Voci 0 e 1 -> toni 1 e 2 (registri A0/B0 e C0/D0); voce 2 -> il suo "periodo" (tabella DS:0290, voci
  oltre la 54a) e' il byte di controllo del rumore (E0h + tipo + velocita'), volume F0h.
  Lista = voci da 3 byte [trasposizione][u16 pattern], chiusa da una parola 0. Pattern:
  00-7F nota (periodo da DS:0290; 60h = silenzio)  80h|d durata  80 n inviluppo (DS:0233[n]: prima del
  puntatore la velocita', poi i livelli fino a FFh)  81 x glissando (x senza segno, sommato al periodo a ogni
  passo)  82 n d vibrato (DS:0276[n], ampiezza periodo/d)  83 vibrato spento  84 fine  FF pattern successivo.
- Tono 0: voce campionata, come nella versione Covox ma scritta sull'attenuazione del canale 0 (90h | nibble)
  dall'interrupt del timer (1000:0415). Lista di puntatori a pattern chiusa da 0; nota = divisore del timer;
  81 i strumento (DS:0396 + 8i: inizio, ripetizione, flag); 82 r p glissando del divisore; il byte 0Fh chiude
  il campione. Il tono 0 non viene mai programmato: si assume l'uscita continua, come nel trucco usato
  sui Tandy per suonare campioni.
Il chip e' emulato sovracampionato x4 e filtrato a 44,1 kHz.

Uso:  tandy_musica.py <radice>   ->  extracted/musica/titolo_tandy.vgm e titolo_tandy.wav
"""
import os, sys, wave, struct
import numpy as np
from scipy.signal import resample_poly
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from adlib_musica import Immagine, PIT

VSYNC = 59.92
CLOCK = 3579545
HZ = 44100
OS = 4

class Voce: pass

def sequenza(im):
    """Restituisce (scritture: lista di (tempo in s, byte per la porta C0h)), durata in s."""
    tempo = im.b(0x3c2); scr = []
    voci = []
    for c in range(3):
        v = Voce(); l = im.w(0x3c6 + 2 * c)
        v.inizio = v.voce = l; v.trasp = im.b(l); v.pc = im.w(l + 1); v.cont = 1
        v.freq = 0; v.env = v.envpos = 0; v.envvel = v.envcnt = 0; v.envon = 0; v.vol = 15; v.vol0 = 15
        v.slide = 0; v.slide_on = 0; v.vib = 0; v.tab = v.tabpos = 0; v.dur = 0
        voci.append(v)
    slot = [[0, 15] for _ in range(3)]                  # DS:02FE + 4c: periodo, volume
    # voce campionata
    plista = im.w(0x3c4); pvoce = plista; ppc = im.w(plista); pcont = 1; pdur = 0; pstrum = 0; pdiv = 0
    pslide = False; prit = 0; ppasso = 0; attivo = False; ptr = 0; rip = 0; flag = 0; parita = im.b(0x390)
    t = 0.0; t_irq = 0.0; fine = False; tick = 0
    while not fine and tick < 60 * 600:
        for c, v in enumerate(voci):
            v.cont = (v.cont - 1) & 0xffff
            if v.cont == 0:
                v.slide_on = 0; pc = v.pc
                while True:
                    a = im.b(pc); pc += 1
                    if not a & 0x80: break
                    if a == 0xff:
                        e = v.voce + 3
                        if im.w(e) == 0: e = v.inizio
                        v.trasp = im.b(e); pc = im.w(e + 1); v.voce = e
                    elif a == 0x80:
                        p = im.w(0x233 + 2 * im.b(pc)); pc += 1
                        v.env = p; v.envvel = im.b(p - 1); v.vol0 = im.b(p)
                    elif a == 0x81: v.slide = im.b(pc); pc += 1; v.slide_on = 1
                    elif a == 0x82: v.tab = im.w(0x276 + 2 * im.b(pc)); v.vib = im.b(pc + 1); pc += 2
                    elif a == 0x83: v.vib = 0
                    elif a == 0x84: fine = True; break
                    else: v.dur = a & 0x3f
                if fine: break
                v.pc = pc
                if a == 0x60:
                    v.vol = 15; slot[c][1] = 15; v.envon = 0
                else:
                    v.freq = im.w(0x290 + 2 * ((a + v.trasp) & 0xff)); slot[c][0] = v.freq
                    v.envpos = v.env; v.envcnt = v.envvel; v.envon = 1; v.tabpos = v.tab
                    slot[c][1] = v.vol0
                v.cont = (v.dur * tempo) & 0xffff
            else:
                if v.envon:
                    v.envcnt = (v.envcnt - 1) & 0xff
                    if v.envcnt == 0:
                        v.envcnt = v.envvel; b = im.b(v.envpos)
                        if b != 0xff:
                            v.envpos = (v.envpos & 0xff00) | ((v.envpos + 1) & 0xff)   # inc byte: solo la parte bassa
                            v.vol = b; slot[c][1] = b
                if v.slide_on: v.freq = (v.freq + v.slide) & 0xffff; slot[c][0] = v.freq
                if v.vib:
                    q = (v.freq // v.vib) & 0xff; q = q - 256 if q > 127 else q
                    d = im.b(v.tabpos)
                    if d == 0x80: v.tabpos = v.tab; d = im.b(v.tabpos)
                    d = d - 256 if d > 127 else d
                    slot[c][0] = (q * d + v.freq) & 0xffff; v.tabpos += 1
        if fine:
            scr += [(t, 0x9f), (t, 0xbf), (t, 0xdf), (t, 0xff)]; break
        for lat, vol, s in ((0xa0, 0xb0, slot[0]), (0xc0, 0xd0, slot[1])):
            n = s[0]; scr += [(t, lat | (n & 0x0f)), (t, (n >> 4) & 0x3f), (t, vol | (s[1] & 0x0f))]
        scr += [(t, slot[2][0] & 0xff), (t, 0xf0 | (slot[2][1] & 0x0f))]
        # voce campionata (1000:0293)
        pcont -= 1
        if pcont == 0:
            pslide = False
            while True:
                a = im.b(ppc); ppc += 1
                if not a & 0x80: break
                if a == 0xff:
                    pvoce += 2
                    if im.w(pvoce) == 0: pvoce = plista
                    ppc = im.w(pvoce)
                elif a == 0x81: pstrum = im.b(ppc); ppc += 1
                elif a == 0x82: pslide = True; prit = im.b(ppc); ppasso = im.b(ppc + 1) | (im.b(0x394) << 8); ppc += 2
                else: pdur = a & 0x3f
            if a == 0x60: attivo = False
            else:
                pdiv = (a << 8) | im.b(ppc); ppc += 1
                ptr = im.w(0x396 + 8 * pstrum); rip = im.w(0x398 + 8 * pstrum); flag = im.b(0x39a + 8 * pstrum)
                attivo = True
            pcont = pdur * tempo
        elif pcont == 1: attivo = False
        elif pslide:
            if prit: prit -= 1
            else: pdiv = (pdiv + ppasso) & 0xffff
        t_fine = t + 1 / VSYNC
        if attivo and pdiv:
            if t_irq < t: t_irq = t
            while t_irq < t_fine and attivo:
                b = im.b(ptr)
                if b == 0x0f:
                    if flag: ptr = rip
                    else: attivo = False
                else:
                    if parita & 1: scr.append((t_irq, 0x90 | (b >> 4)))
                    else: scr.append((t_irq, 0x90 | (b & 0x0f))); ptr += 1
                parita += 1
                t_irq += pdiv / PIT
        t = t_fine; tick += 1
    return scr, t

LFSR = None
def rumore_bianco():
    global LFSR
    if LFSR is None:       # SN76496: registro a 16 bit, prese sui bit 1 e 2 (feedback 0x0006)
        r = 0x8000; out = np.empty(65535, np.int8)
        for i in range(65535):
            out[i] = r & 1
            fb = ((r >> 1) ^ (r >> 2)) & 1
            r = (r >> 1) | (fb << 15)
        LFSR = out
    return LFSR

def emula(scr, durata):
    """Emulazione dello SN76496 a HZ x OS, a tratti costanti fra una scrittura e l'altra."""
    R = HZ * OS; n_tot = int(durata * R) + 1
    out = np.zeros(n_tot, np.float32)
    per = [0x400, 0x400, 0x400, 0]; att = [15, 15, 15, 15]; ctrl = 0; latch = 0
    fase = [0.0, 0.0, 0.0]; passi_rumore = 0.0; reset = False
    amp = np.array([10 ** (-2 * v / 20) for v in range(15)] + [0.0], np.float32)
    lfsr = rumore_bianco(); cyc = CLOCK / 16 / R               # passi del contatore del chip per campione
    i = 0; k = 0; eventi = sorted(scr, key=lambda e: e[0])
    while k <= len(eventi):
        t_ev = eventi[k][0] if k < len(eventi) else durata
        j = min(n_tot, int(t_ev * R))
        if j > i:
            n = j - i; idx = np.arange(n)
            seg = np.zeros(n, np.float32)
            for c in range(3):
                if att[c] == 15: fase[c] += cyc * n; continue
                if c == 0: seg += amp[att[0]]; continue                 # tono 0 continuo: canale PCM
                N = per[c] or 0x400
                if N <= 1: seg += amp[att[c]]; continue
                f = fase[c] + cyc * idx
                seg += amp[att[c]] * np.where((np.floor(f / N).astype(np.int64) & 1) == 0, 1.0, -1.0)
                fase[c] += cyc * n
            if att[3] != 15:
                r = ctrl & 3; N = (0x10, 0x20, 0x40)[r] if r < 3 else (per[2] or 0x400)
                s = passi_rumore + cyc * idx / N
                st = np.floor(s).astype(np.int64)
                if ctrl & 4: bit = lfsr[st % len(lfsr)]
                else: bit = (st % 16 == 0).astype(np.int8)
                seg += amp[att[3]] * (bit * 2 - 1)
                passi_rumore += cyc * n / N
            out[i:j] = seg; i = j
        if k == len(eventi): break
        b = eventi[k][1]; k += 1
        if b & 0x80:
            latch = (b >> 4) & 7; ch = latch >> 1
            if latch & 1: att[ch] = b & 0x0f
            elif ch < 3: per[ch] = (per[ch] & 0x3f0) | (b & 0x0f)
            else: ctrl = b & 7; passi_rumore = 0.0
        else:
            ch = latch >> 1
            if latch & 1: att[ch] = b & 0x0f
            elif ch < 3: per[ch] = (per[ch] & 0x0f) | ((b & 0x3f) << 4)
            else: ctrl = b & 7
    y = resample_poly(out, 1, OS)
    y = y / max(1e-9, np.abs(y).max()) * 0.89                   # normalizzato a -1 dBFS
    return np.clip(y * 32767, -32768, 32767).astype('<i2')

def vgm(scr, durata):
    dati = bytearray(); ora = 0
    for t, b in sorted(scr, key=lambda e: e[0]):
        s = int(t * HZ)
        while s > ora:
            k = min(s - ora, 65535); dati += bytes((0x61,)) + struct.pack('<H', k); ora += k
        dati += bytes((0x50, b))
    fine = int(durata * HZ)
    while fine > ora:
        k = min(fine - ora, 65535); dati += bytes((0x61,)) + struct.pack('<H', k); ora += k
    dati += b'\x66'
    h = bytearray(0x40)
    h[0:4] = b'Vgm '
    struct.pack_into('<I', h, 0x04, 0x40 + len(dati) - 4); struct.pack_into('<I', h, 0x08, 0x151)
    struct.pack_into('<I', h, 0x0c, CLOCK); struct.pack_into('<I', h, 0x18, ora)
    struct.pack_into('<H', h, 0x28, 0x0006); h[0x2a] = 16                  # SN76496: feedback e registro
    struct.pack_into('<I', h, 0x34, 0x40 - 0x34)
    return bytes(h + dati)

if __name__ == '__main__':
    root = sys.argv[1]; im = Immagine(root)
    scr, durata = sequenza(im)
    dst = f'{root}/extracted/musica'; os.makedirs(dst, exist_ok=True)
    open(f'{dst}/titolo_tandy.vgm', 'wb').write(vgm(scr, durata))
    pcm = emula(scr, durata)
    with wave.open(f'{dst}/titolo_tandy.wav', 'wb') as w:
        w.setnchannels(1); w.setsampwidth(2); w.setframerate(HZ); w.writeframes(pcm.tobytes())
    print(f'durata {durata:.1f} s  scritture {len(scr)}  campioni {len(pcm)}')
