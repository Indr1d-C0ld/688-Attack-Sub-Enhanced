"""Genera la tabella Markdown degli opcode della VM (da tok_ops.py e re/vm_lunghezze.json)."""
import os, sys, json
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from tok_ops import OPS, SIZE

TIPI = {'b': 'byte', 'w': 'word', 'u': 'word s.s.', 'L': 'etichetta', 'T': 'tempo', 'H': 'gestore', 'V': 'variabile'}
lun = json.load(open(sys.argv[1]))
print('| Op | Mnemonico | Argomenti | Byte | Salto | Gestore | Effetto |')
print('|---|---|---|---|---|---|---|')
for op in sorted(OPS):
    name, spec, desc = OPS[op]
    args = ', '.join(TIPI[c] for c in spec) or '—'
    n = 1 + sum(SIZE[c] for c in spec)
    info = lun.get(f'{op:#04x}', {})
    salto = 'sì' if info.get('consumo_salto') or 'L' in spec and op in (0x43, 0x01, 0x14) else ''
    gest = info.get('gestore', '—')
    print(f'| `{op:02X}` | `{name}` | {args} | {n} | {salto} | `{gest}` | {desc} |')
