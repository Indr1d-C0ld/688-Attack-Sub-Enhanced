"""Disassemblatore dei file SCENE*.TOK di 688 Attack Sub (bytecode della VM degli scenari).

Uso:  tok_disasm.py <cartella con SCENE*.TOK> <cartella di uscita> [re/vm_lunghezze.json]

Disassemblaggio guidato dal flusso: parte dall'offset 0 e da ogni etichetta (salti, CALL, timer,
gestori di evento). Per ogni file scrive <scena>.asm e alla fine riporta i byte non coperti dal
flusso; questi vengono poi provati come codice morto (sequenze decodificabili fino a END) e
altrimenti marcati come dati.
"""
import os, sys, glob, json, struct
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from tok_ops import OPS, SIZE, TERMINA

def oplen(op):
    return 1 + sum(SIZE[c] for c in OPS[op][1])

def verifica_lunghezze(path):
    t = json.load(open(path))
    errori = []
    for k, v in t.items():
        op = int(k, 16)
        attesa = (v['consumo_ritorno'] or v['consumo_salto'])[0]
        if oplen(op) - 1 != attesa:
            errori.append((k, OPS[op][0], oplen(op) - 1, attesa))
    return errori

def decode(d, pos):
    op = d[pos]
    if op not in OPS:
        return None
    name, spec, _ = OPS[op]
    p = pos + 1; args = []
    for c in spec:
        if p + SIZE[c] > len(d):
            return None
        if c == 'b':
            args.append((c, d[p]))
        else:
            v = struct.unpack_from('<H', d, p)[0]
            if c == 'w' and v >= 0x8000: v -= 0x10000
            args.append((c, v))
        p += SIZE[c]
    return op, name, args, p

def bersagli(op, args):
    out = []
    for c, v in args:
        if c in 'LH':
            out.append(v)
    # 0x65: nel codice originale il ramo "uguale" riprende a etichetta + 2 (difetto: a meta'
    # dell'istruzione); il flusso segue l'etichetta scritta e il difetto e' annotato nel listato
    return out

def disasm(d):
    code = {}                 # offset -> (op, name, args, next)
    labels = {0: 'inizio'}
    kind = {}
    work = [0]
    while work:
        pos = work.pop()
        while 0 <= pos < len(d) and pos not in code:
            r = decode(d, pos)
            if r is None:
                kind.setdefault(pos, 'non decodificabile'); break
            op, name, args, nxt = r
            code[pos] = r
            for c, v in args:
                if c in 'LHT' and c != 'T':
                    pass
            for t in bersagli(op, args):
                if 0 <= t < len(d):
                    lab = ('H' if any(c == 'H' for c, _ in args) else 'L') + f'_{t:04x}'
                    labels.setdefault(t, lab); work.append(t)
                else:
                    kind.setdefault(t, f'etichetta fuori dal file da {pos:04x}')
            if op in TERMINA:
                break
            pos = nxt
    covered = bytearray(len(d))
    for p, (op, name, args, nxt) in code.items():
        for i in range(p, nxt): covered[i] = 1
    # buchi: prova come codice morto, altrimenti dati
    gaps = []
    i = 0
    while i < len(d):
        if covered[i]: i += 1; continue
        j = i
        while j < len(d) and not covered[j]: j += 1
        gaps.append((i, j)); i = j
    dead = {}
    for a, b in gaps:
        p = a; ok = True; tmp = {}
        while p < b:
            r = decode(d, p)
            if r is None or r[3] > b: ok = False; break
            tmp[p] = r; p = r[3]
        if ok:
            dead.update(tmp)
    return code, labels, gaps, dead, kind

def carica_tabella(nome):
    """Tabelle testuali generate da GAME.EXE (re/tok/messaggi.txt, re/tok/tipi_unita.txt)."""
    t = {}
    path = os.path.join(os.path.dirname(os.path.abspath(__file__)), '../../re/tok', nome)
    if os.path.exists(path):
        for riga in open(path):
            if riga.startswith('0x'):
                k, _, v = riga.partition(" "); t[int(k, 16)] = v.strip()
    return t

MESSAGGI = carica_tabella('messaggi.txt')
TIPI = carica_tabella('tipi_unita.txt')

def commento(op, args):
    if op in (0x55, 0x23) and args:
        c = args[0][1]
        if c >= 100: return f'esito di fine missione {c - 100}'
        return MESSAGGI.get(c, '')
    if op == 0x28 and args:
        return TIPI.get(args[0][1], '')
    if op == 0x65:
        return "difetto originale: se uguali riprende a etichetta+2, a meta' istruzione"
    return ''

def fmt_args(op, args, labels):
    out = []
    for c, v in args:
        if c in 'LH':
            out.append(labels.get(v, f'{v:#06x}'))
        elif c == 'T':
            out.append(f'@{v & 0x7fff}' if v & 0x8000 else f'+{v}')
        elif c == 'V':
            out.append(f'var[{v}]')
        elif c == 'b':
            out.append(f'{v:#04x}')
        else:
            out.append(str(v))
    return ', '.join(out)

def scrivi(nome, d, code, labels, gaps, dead, out):
    with open(out, 'w') as f:
        f.write(f'; {nome}: {len(d)} byte, {len(code)} istruzioni raggiunte dal flusso\n')
        tutte = dict(dead); tutte.update(code)
        i = 0
        while i < len(d):
            if i in labels:
                f.write(f'\n{labels[i]}:\n')
            if i in tutte:
                op, name, args, nxt = tutte[i]
                note = [x for x in (commento(op, args), 'codice non raggiunto' if i not in code else '') if x]
                raw = d[i:nxt].hex(' ')
                txt = fmt_args(op, args, labels)
                f.write(f'  {i:04x}  {raw:<32s} {name:<15s} {txt}' + (f'{"":<{max(1, 28 - len(txt))}}; ' + ' | '.join(note) if note else '') + '\n')
                i = nxt
            else:
                j = i
                while j < len(d) and j not in tutte and (j == i or j not in labels): j += 1
                f.write(f'  {i:04x}  {d[i:j].hex(" "):<32s} .dati\n')
                i = j

if __name__ == '__main__':
    src, dst = sys.argv[1], sys.argv[2]
    lun = sys.argv[3] if len(sys.argv) > 3 else os.path.join(os.path.dirname(__file__), '../../re/vm_lunghezze.json')
    err = verifica_lunghezze(lun)
    if err:
        sys.exit(f'lunghezze della tabella non coerenti con il codice: {err}')
    os.makedirs(dst, exist_ok=True)
    tot = {'file': 0, 'istr': 0, 'dati': 0, 'morto': 0}
    for fpath in sorted(glob.glob(os.path.join(src, 'SCENE*.TOK'))):
        nome = os.path.splitext(os.path.basename(fpath))[0]
        d = open(fpath, 'rb').read()
        code, labels, gaps, dead, kind = disasm(d)
        scrivi(nome, d, code, labels, gaps, dead, os.path.join(dst, nome + '.asm'))
        dati = sum(b - a for a, b in gaps) - sum(r[3] - p for p, r in dead.items())
        morto = sum(r[3] - p for p, r in dead.items())
        cov = sum(r[3] - p for p, r in code.items())
        print(f'{nome}: {len(d):5d} byte  flusso {cov:5d}  codice morto {morto:4d}  dati {dati:4d}  '
              f'etichette {len(labels):3d}  anomalie {len(kind)}' + (f' {sorted(kind.items())[:3]}' if kind else ''))
        tot['file'] += 1; tot['istr'] += len(code); tot['dati'] += dati; tot['morto'] += morto
    print('totale', tot)
