"""Musica dei titoli nella versione Covox Speech Thing (CALLS.EXE, 1000:05FC): render in WAV.

Una sola voce campionata. Il sequencer (1000:06BA) gira a ogni ritorno verticale del video (il ciclo
principale aspetta il bit 3 della porta 3DAh): con la schermata dei titoli in modo 13h sono 70,09 Hz.
Dati: DS:06E4 tempo, DS:06E6 lista di puntatori a pattern (chiusa da 0, si ricomincia). Pattern:
  00-7F nota = divisore del timer a 16 bit (byte alto, poi basso): frequenza di campionamento
  1193182 / divisore; 60h = pausa      80h|d durata d (x tempo)      81 i strumento i
  82 r p glissando: dopo r passi il divisore cresce di p a ogni passo      84 fine      FF pattern successivo
Strumenti (DS:06B8 + 8i): inizio, punto di ripetizione, flag di ripetizione. A ogni interrupt del timer
(1000:07C6) esce un nibble del campione (prima l'alto, poi il basso, x 16) sulla porta della stampante
LPT1; il byte 0Fh chiude il campione (si riparte dal punto di ripetizione se il flag e' acceso, altrimenti
il suono si ferma). L'interrupt si spegne anche al penultimo passo di ogni nota.

Uso:  covox_musica.py <radice>   ->  extracted/musica/titolo_covox.wav (8 bit, 44,1 kHz)
"""
import os, sys, wave, struct
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from adlib_musica import Immagine, PIT

VSYNC = 70.086
HZ = 44100

def render(im):
    tempo = im.b(0x6e4); lista = im.w(0x6e6); voce = lista; pc = im.w(lista)
    cont = 1; dur = 0; strum = 0; div = 0; slide = False; ritardo = 0; passo_slide = 0
    attivo = False; ptr = 0; ripeti = 0; flag_rip = 0; parita = 0; dac = 0x80
    out = bytearray(); t_irq = 0.0; t = 0.0; fine = False; tick = 0; giro = None
    while not fine and tick < 70 * 600:
        # --- sequencer (1000:06BA)
        cont -= 1
        if cont == 0:
            slide = False
            while True:
                a = im.b(pc); pc += 1
                if not a & 0x80: break
                if a == 0xff:
                    voce += 2
                    if im.w(voce) == 0:
                        voce = lista
                        if giro is None: giro = tick
                    pc = im.w(voce)
                elif a == 0x81: strum = im.b(pc); pc += 1
                elif a == 0x82: slide = True; ritardo = im.b(pc); passo_slide = im.w(pc + 1) & 0xff | (im.b(0x6b6) << 8); pc += 2
                elif a == 0x84: fine = True; break
                else: dur = a & 0x3f
            if fine: break
            if a == 0x60: attivo = False
            else:
                div = (a << 8) | im.b(pc); pc += 1
                ptr = im.w(0x6b8 + 8 * strum); ripeti = im.w(0x6ba + 8 * strum); flag_rip = im.b(0x6bc + 8 * strum)
                attivo = True
            cont = dur * tempo
        elif cont == 1: attivo = False
        elif slide:
            if ritardo == 0: div = (div + passo_slide) & 0xffff
            else: ritardo -= 1
        # --- interrupt del timer per la durata del passo (1000:07C6), DAC tenuto fra un campione e l'altro
        t_fine = t + 1 / VSYNC
        if attivo and div:
            periodo = div / PIT
            if t_irq < t: t_irq = t
            while t_irq < t_fine and attivo:
                n = int(t_irq * HZ) - len(out)
                if n > 0: out += bytes((dac,)) * n
                b = im.b(ptr)
                if b == 0x0f:
                    if flag_rip: ptr = ripeti
                    else: attivo = False
                else:
                    if parita & 1: dac = (b & 0x0f) << 4; ptr += 1
                    else: dac = b & 0xf0
                parita += 1
                t_irq += periodo
        t = t_fine; tick += 1
        n = int(t * HZ) - len(out)
        if n > 0: out += bytes((dac,)) * n
    return out, tick, giro

if __name__ == '__main__':
    root = sys.argv[1]; im = Immagine(root)
    pcm, tick, giro = render(im)
    dst = f'{root}/extracted/musica'; os.makedirs(dst, exist_ok=True)
    with wave.open(f'{dst}/titolo_covox.wav', 'wb') as w:
        w.setnchannels(1); w.setsampwidth(1); w.setframerate(HZ); w.writeframes(bytes(pcm))
    print(f'tick {tick} ({tick / VSYNC:.1f} s)  ripetizione della lista al tick {giro}  campioni {len(pcm)}')
