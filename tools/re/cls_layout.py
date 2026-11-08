"""Formato .CLS di 688 Attack Sub: liste di comandi grafiche delle stazioni e dei dialoghi.

Ricavato dall'interprete di GAME.EXE (1000:BB44 carica una pagina, 1000:BBBD esegue i comandi,
1000:BC29 decodifica gli argomenti).

Contenitore: u8 n; n parole u16 = offset (dal file) delle voci 1..n; parola u16 = offset della base del
codice. Voce = [u8 numero di argomenti][u16 offset del codice dalla base][u16 tipi, 2 bit per argomento,
il primo nei bit alti del primo byte: 1 byte, 2 parola, 3 stringa terminata da zero]. La pagina 0 e' il
codice all'inizio della base (lo schermo); le voci sono pagine richiamate dal gioco con argomenti (per
esempio STAT.CLS pagina lato+1) e macro richiamate dal file stesso.

Comando: u8 op. 0 = fine; 1..7Fh = primitiva del gioco (tabella a DS:97A2, 31 voci con lo stesso formato
delle voci del file); 80h+k = macro k del file. Se l'op ha argomenti: un byte di modi (due se gli argomenti
sono piu' di 4), 2 bit per argomento: 0 = valore letterale, 1 = argomento della macro chiamante,
2 = argomento + letterale, 3 = argomento - letterale; poi i nibble che scelgono l'argomento della chiamante
(alto, poi basso); poi i letterali nell'ordine, del tipo indicato dalla voce.

Primitive usate per il testo (gestori in 1000:D630 e seguenti): 6 = posizione del cursore (x, y);
5 = scrive una stringa; 8 = la scrive in rilievo (ombra a +1,+1, colore dell'ombra nel secondo byte);
12 = colore; 13 = spaziatura tra le lettere; 27 = finestra del testo (x1, y1, x2, y2): oltre x2 il testo va
a capo a meta' parola. Nella stringa "@" accende e spegne il grassetto (ogni carattere ridisegnato a +1,
quindi largo 1 pixel in piu'), "@@" e' una chiocciola, "\\n" va a capo.
Font: uno solo, proporzionale, a DS:9D5C in GAME.EXE: caratteri da 14 byte (larghezza + 13 righe) dal codice
20h; da 80h in poi sono icone e cifre grandi (il tratteggio di DETONATE, le frecce, i simboli delle stazioni).
Le maiuscole sono piccole (5x5), gli slot delle minuscole contengono maiuscole grandi (7x7).

Uso:  cls_layout.py <radice>   ->  re/cls/<FILE>.txt (listato), re/cls/<FILE>.png (pagina 0 ridisegnata)
e re/cls/testi.json (ogni testo con posizione, larghezza e spazio disponibile), letto da trad_spazi.py.
"""
import os, sys, glob, json, struct
from PIL import Image, ImageDraw

DS = 0x13060
NATIVI = {1: 'rettangolo', 2: 'linea_v', 3: 'linea_o', 4: 'pixel', 5: 'testo', 6: 'cursore', 7: 'casella_numero',
          8: 'testo_rilievo', 9: 'cornice', 10: 'area_attiva', 11: 'tabella', 12: 'colore', 13: 'spaziatura',
          14: 'n14', 15: 'carica_cls', 16: 'n16', 17: 'n17', 18: 'n18', 19: 'n19', 20: 'n20', 21: 'n21',
          22: 'n22', 23: 'n23', 24: 'area_mouse', 25: 'n25', 26: 'testo2', 27: 'finestra_testo',
          28: 'immagine', 29: 'n29', 30: 'n30', 31: 'n31'}
EGA = [(0, 0, 0), (0, 0, 170), (0, 170, 0), (0, 170, 170), (170, 0, 0), (170, 0, 170), (170, 85, 0), (170, 170, 170),
       (85, 85, 85), (85, 85, 255), (85, 255, 85), (85, 255, 255), (255, 85, 85), (255, 85, 255), (255, 255, 85),
       (255, 255, 255)]
PAR = 100000   # gli argomenti simbolici di una pagina valgono PAR*(i+1) + scostamento

def carica_exe(root):
    d = open(f'{root}/re/unpacked/GAME.EXE', 'rb').read()
    return d[struct.unpack_from('<H', d, 8)[0] * 16:]

def font(img):
    t = DS + 0x9d5c
    return {chr(c): (img[t + (c - 32) * 14], img[t + (c - 32) * 14 + 1:t + (c - 32) * 14 + 14]) for c in range(32, 256)}

def voce(buf, tab, i):
    o = struct.unpack_from('<H', buf, tab + 1 + 2 * i)[0]
    e = tab + o
    na = buf[e]; h, t = struct.unpack_from('<HH', buf, e + 1)
    return na, h, ((t & 0xff) << 8) | (t >> 8)

def larghezza(s, fnt, spazio=1, grassetto=False, rilievo=False):
    """Larghezza in pixel di una riga come la disegna 1000:D630 (accenti contati come la lettera base)."""
    base = str.maketrans('ÀÁÈÉÌÍÒÓÙÚàáèéìíòóùú', 'AAEEIIOOUUaaeeiioouu')
    s = s.translate(base); w = 0; i = 0; n = 0
    while i < len(s):
        c = s[i]
        if c == '@' and s[i + 1:i + 2] != '@': grassetto = not grassetto; i += 1; continue
        if c == '@': i += 1
        g = fnt.get(c, fnt['?'])
        w += g[0] + spazio + (1 if grassetto else 0); n += 1; i += 1
    return (w - spazio if n else 0) + (1 if rilievo else 0)

class Esecutore:
    def __init__(self, img, fnt, f, nome):
        self.img, self.fnt, self.f, self.nome = img, fnt, f, nome
        self.base = struct.unpack_from('<H', f, 1 + 2 * f[0])[0]
        self.nat = DS + 0x97a2
        self.eventi = []; self.listato = []
        self.cx = self.cy = 0; self.spazio = 1; self.colore = 15; self.finestra = (0, 0, 319, 199); self.riga = 9

    def esegui(self, pc, args, pila):
        f = self.f
        while True:
            op = f[pc]; pc += 1
            if op == 0: return
            if op < 0x80: na, h, tipi = voce(self.img, self.nat, op - 1)
            else: na, h, tipi = voce(f, 0, op & 0x7f)
            val = [None] * na
            if na:
                m = f[pc]; pc += 1
                modi = (m << 8) | m
                if na > 4: modi = (m << 8) | f[pc]; pc += 1
                mm = [(modi >> (14 - 2 * k)) & 3 for k in range(na)]
                alto = True; basso = 0
                for k in range(na):
                    if mm[k]:
                        if alto: b = f[pc]; pc += 1; nib = b >> 4; basso = b & 15
                        else: nib = basso
                        alto = not alto
                        val[k] = args[nib] if nib < len(args) else 0
                for k in range(na):
                    t = (tipi >> (14 - 2 * k)) & 3
                    if mm[k] == 1: continue
                    if t == 1: v = f[pc]; pc += 1
                    elif t == 2: v = struct.unpack_from('<H', f, pc)[0]; pc += 2
                    else: e = f.index(0, pc); v = f[pc:e].decode('latin-1'); pc = e + 1
                    val[k] = v if mm[k] == 0 else (val[k] or 0) + v if mm[k] == 2 else (val[k] or 0) - v
            if op >= 0x80:
                self.listato.append('  ' * len(pila) + f'M{op & 0x7f} {val}')
                self.esegui(self.base + h, val, pila + [op & 0x7f])
            else:
                self.listato.append('  ' * len(pila) + f'{NATIVI.get(op, op)} {val}')
                self.primitiva(op, val, pila)

    def primitiva(self, op, v, pila):
        ev = dict(op=NATIVI.get(op, str(op)), args=v, pila=list(pila))
        if op == 6: self.cx, self.cy = v
        elif op == 12: self.colore = v[0]
        elif op == 13: self.spazio = v[0]
        elif op == 27: self.finestra = tuple(v); self.cx, self.cy = v[0], v[1]
        elif op in (5, 26, 8) and not isinstance(v[0], str):
            ev.update(x=self.cx, y=self.cy, testo_da_argomento=True)    # stringa passata dal gioco alla pagina
        elif op in (5, 26, 8):
            ev.update(x=self.cx, y=self.cy, spazio=self.spazio, finestra=self.finestra, colore=self.colore,
                      rilievo=op == 8, righe=[])
            grassetto = False
            for k, r in enumerate(v[0].split('\n')):
                if k: self.cx = self.finestra[0]; self.cy += self.riga
                w = larghezza(r, self.fnt, self.spazio, grassetto, op == 8)
                ev['righe'].append(dict(testo=r, x=self.cx, y=self.cy, w=w))
                grassetto ^= r.replace('@@', '').count('@') % 2 == 1
                self.cx += w + self.spazio
        self.eventi.append(ev)

def esegui_file(img, fnt, path):
    f = open(path, 'rb').read(); n = f[0]; nome = os.path.basename(path)
    pagine = {}
    e = Esecutore(img, fnt, f, nome); e.esegui(e.base, [], []); pagine[0] = e
    for p in range(1, n + 1):
        na, h, _ = voce(f, 0, p - 1)
        e = Esecutore(img, fnt, f, nome)
        try: e.esegui(e.base + h, [PAR * (i + 1) for i in range(na)], [p - 1])
        except (IndexError, ValueError) as x: e.listato.append(f'ERRORE {x}')
        pagine[p] = e
    return pagine

def simbolico(v):
    return isinstance(v, int) and v > PAR // 2

def disegna(root, ev, fnt, out):
    im = Image.new('RGB', (320, 200), (0, 0, 0)); d = ImageDraw.Draw(im)
    col = lambda c: EGA[c & 15] if isinstance(c, int) else (255, 0, 255)
    for e in ev:
        a = e['args']
        if any(simbolico(x) for x in a): continue
        o = e['op']
        if o == 'rettangolo': d.rectangle([min(a[0], a[2]), min(a[1], a[3]), max(a[0], a[2]), max(a[1], a[3])], fill=col(a[4]))
        elif o == 'cornice': d.rectangle([min(a[0], a[2]), min(a[1], a[3]), max(a[0], a[2]), max(a[1], a[3])], outline=col(a[4]))
        elif o == 'linea_v': d.line([a[0], a[1], a[0], a[2]], fill=col(a[3]))
        elif o == 'linea_o': d.line([a[1], a[0], a[2], a[0]], fill=col(a[3]))
        elif o == 'pixel': d.point([a[0], a[1]], fill=col(a[2]))
        elif o == 'casella_numero':
            d.rectangle([a[1], a[2], a[1] + 10, a[2] + 10], outline=(255, 255, 255))
            scrivi(d, fnt, str(a[0]), a[1] + 2, a[2] + 2, (255, 255, 255), 1)
        elif o == 'immagine':
            p = f"{root}/extracted/png/{a[0].upper().replace('.ENC', '.png')}"
            if os.path.exists(p) and not simbolico(a[1]): im.paste(Image.open(p).convert('RGB'), (a[1], a[2]))
        elif 'righe' in e:
            for r in e['righe']:
                if e['rilievo']: scrivi(d, fnt, r['testo'], r['x'] + 1, r['y'] + 1, col(e['args'][2]), e['spazio'])
                scrivi(d, fnt, r['testo'], r['x'], r['y'], col(e['args'][1] if e['rilievo'] else e['colore']), e['spazio'])
    im.resize((640, 400), Image.NEAREST).save(out)

def scrivi(d, fnt, s, x, y, c, spazio):
    grassetto = False; i = 0
    while i < len(s):
        ch = s[i]
        if ch == '@' and s[i + 1:i + 2] != '@': grassetto = not grassetto; i += 1; continue
        if ch == '@': i += 1
        w, rr = fnt.get(ch, fnt['?'])
        for dx in ((0, 1) if grassetto else (0,)):
            for ry, b in enumerate(rr):
                for bx in range(8):
                    if b & (0x80 >> bx): d.point([x + bx + dx, y + ry], fill=c)
        x += w + spazio + (1 if grassetto else 0); i += 1

if __name__ == '__main__':
    root = sys.argv[1]; img = carica_exe(root); fnt = font(img)
    dst = f'{root}/re/cls'; os.makedirs(dst, exist_ok=True)
    testi = {}
    for path in sorted(glob.glob(f'{root}/extracted/raw/DISK*/*.CLS')):
        nome = os.path.basename(path); pagine = esegui_file(img, fnt, path)
        with open(f'{dst}/{nome}.txt', 'w') as out:
            for p, e in pagine.items():
                out.write(f'== pagina {p}\n' + '\n'.join(e.listato) + '\n\n')
        disegna(root, pagine[0].eventi, fnt, f'{dst}/{nome}.png')
        testi[nome] = {str(p): e.eventi for p, e in pagine.items()}
        print(f'{nome:12s} pagine {len(pagine) - 1:3d}  comandi {sum(len(e.listato) for e in pagine.values()):5d}')
    json.dump(dict(font={c: g[0] for c, g in fnt.items()}, file=testi), open(f'{dst}/eventi.json', 'w'), ensure_ascii=False)
