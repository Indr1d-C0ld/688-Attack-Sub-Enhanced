"""Analisi dei gestori della VM degli scenari (GAME.EXE decompresso, segmento 2011h).

Per ogni opcode esplora tutti i percorsi del gestore (salti condizionati, jmp, call locali
annidate) e calcola quanti byte di argomento vengono consumati da SI (lodsb +1, lodsw +2,
rep movsw con CX costante +2*CX) prima del ritorno. Un percorso che esegue
`lea si,[0x5105]` + `add si,ax` e' un salto: SI viene riposizionato nel bytecode.
Uscita: listato completo per opcode (istruzioni raggiungibili) e tabella JSON delle lunghezze.
"""
import struct, sys, json, capstone

d = open(sys.argv[1], 'rb').read()
img = d[struct.unpack_from('<H', d, 8)[0] * 16:]
DS = 0x13060; SEG = (0x2011 - 0x1000) * 16
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_16)

def ins_at(a):
    return next(md.disasm(img[SEG + a:SEG + a + 16], a))

handlers = []
for i in range(256):
    v = struct.unpack_from('<H', img, DS + 0x6505 + 2 * i)[0]
    if not (0x1300 <= v < 0x3000): break
    handlers.append(v)

def imm(op):
    try: return int(op, 16) if op.startswith('0x') else int(op)
    except ValueError: return None

def explore(start, depth=0, seen_calls=()):
    """Ritorna (insiemi di esiti, indirizzi visitati). Esito: ('ret', consumo) | ('salto', consumo) | ('?', motivo)."""
    outcomes = set(); visited = set(); stati = set()
    # stato: (indirizzo, consumo, cx noto, passi, si_sul_bytecode, pila dei push si)
    stack = [(start, 0, None, 0, True, ())]
    while stack:
        a, used, cx, steps, onbc, saved = stack.pop()
        while True:
            key = (a, used, cx, onbc, saved)
            if key in stati: break                    # stato gia' esplorato: evita l'esplosione dei percorsi
            stati.add(key)
            if steps > 400 or len(stati) > 20000: outcomes.add(('?', 'troppo lungo')); break
            i = ins_at(a); visited.add(a); steps += 1
            m, op = i.mnemonic, i.op_str
            nxt = a + i.size
            if m == 'push' and op == 'si': saved = saved + ((used, onbc),)
            elif m == 'pop' and op == 'si' and saved: used, onbc = saved[-1]; saved = saved[:-1]
            elif (m in ('mov', 'lea', 'les', 'lds') and op.startswith('si, ') and not op.startswith('si, [0x5105]')) \
                    or (m == 'xchg' and 'si' in op.split(', ')):
                onbc = False                          # SI non punta piu' al bytecode (lista, struttura, ...)
            if m == 'lodsb' and onbc: used += 1
            elif m == 'lodsw' and onbc: used += 2
            elif onbc and (m == 'rep movsw' or (m.startswith('rep') and 'movsw' in m)):
                if 'word ptr [si]' in op:
                    used += 2 * cx if cx is not None else 0
                    if cx is None: outcomes.add(('?', 'rep movsw cx ignoto'))
            if m == 'mov' and op.startswith('cx, '):
                cx = imm(op[4:])
            if m == 'lea' and op.startswith('si, [0x5105]'):
                outcomes.add(('salto', used)); break
            if m in ('ret', 'retf'):
                outcomes.add(('ret', used)); break
            if m == 'int3':
                outcomes.add(('ret', used)); break
            if m == 'jmp':
                t = imm(op)
                if t is None: outcomes.add(('?', 'jmp indiretto ' + op)); break
                a = t; continue
            if m.startswith('j') or m in ('loop', 'jcxz'):
                t = imm(op)
                if t is not None: stack.append((t, used, cx, steps, onbc, saved))
                a = nxt; continue
            if m == 'call':
                t = imm(op)
                if t is not None and depth < 4 and t not in seen_calls and onbc:
                    sub, vis = explore(t, depth + 1, seen_calls + (t,))
                    visited |= vis
                    extra = {u for k, u in sub if k == 'ret'}
                    if any(k == 'salto' for k, u in sub): outcomes.add(('salto-in-call', used))
                    if len(extra) == 1: used += extra.pop()
                    elif len(extra) > 1: outcomes.add(('?', f'call {t:#x} consumo variabile {sorted(extra)}'))
                a = nxt; continue
            a = nxt
    return outcomes, visited

tab = {}
out = open(sys.argv[2], 'w')
out.write('; Gestori della VM degli scenari: istruzioni raggiungibili (call locali incluse)\n')
for op, h in enumerate(handlers, 1):
    oc, vis = explore(h)
    rets = sorted({u for k, u in oc if k == 'ret'}); jumps = sorted({u for k, u in oc if k in ('salto', 'salto-in-call')})
    unk = sorted({u for k, u in oc if k == '?'})
    tab[f'{op:#04x}'] = dict(gestore=f'2011:{h:04x}', consumo_ritorno=rets, consumo_salto=jumps, note=unk)
    out.write(f'\n;===== opcode {op:#04x} -> 2011:{h:04x}  ritorno:{rets} salto:{jumps} {unk}\n')
    for a in sorted(vis):
        i = ins_at(a)
        out.write(f'  {a:04x}  {i.mnemonic:8s} {i.op_str}\n')
json.dump(tab, open(sys.argv[3], 'w'), indent=1)
for k, v in tab.items():
    flag = '' if len(v['consumo_ritorno']) <= 1 and not v['note'] else '  <-- da verificare'
    print(k, v['gestore'], 'ritorno', v['consumo_ritorno'], 'salto', v['consumo_salto'], v['note'], flag)
