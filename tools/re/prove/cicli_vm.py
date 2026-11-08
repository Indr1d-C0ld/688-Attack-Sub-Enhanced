"""Cicli senza uscita nel bytecode delle scene: salti all'indietro che tornano su un'istruzione gia' eseguita senza
END, GOTO in avanti o istruzioni che chiudono lo script (TERMINA). Uso: cicli_vm.py <radice>"""
import glob, os, sys
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
from tok_disasm import decode
from tok_ops import TERMINA

def successori(d, pc):
    r = decode(d, pc)
    if r is None: return r, []
    op, name, args, nxt = r
    if name == 'END' or op in TERMINA: return r, []
    if name == 'GOTO': return r, [args[0][1]]
    s = [nxt]
    if name.startswith('IF_'): s += [v for c, v in args if c == 'L']
    return r, s

for f in sorted(glob.glob(f'{sys.argv[1]}/extracted/raw/DISK*/SCENE*.TOK')):
    d = open(f, 'rb').read()
    # componenti fortemente connesse del grafo delle istruzioni raggiungibili da ogni offset decodificabile
    grafo = {}
    for pc in range(len(d)):
        r, s = successori(d, pc)
        if r is not None: grafo[pc] = [x for x in s if 0 <= x < len(d)]
    # solo il codice raggiungibile dai bersagli dei salti e dall'inizio
    radici = {0} | {x for s in grafo.values() for x in s}
    raggiunti = set(); pila = list(radici)
    while pila:
        pc = pila.pop()
        if pc in raggiunti or pc not in grafo: continue
        raggiunti.add(pc); pila += grafo[pc]
    indice = {}; basso = {}; pila = []; in_pila = set(); n = [0]; cicli = []
    sys.setrecursionlimit(100000)
    def visita(v):
        indice[v] = basso[v] = n[0]; n[0] += 1; pila.append(v); in_pila.add(v)
        for w in grafo.get(v, []):
            if w not in raggiunti: continue
            if w not in indice: visita(w); basso[v] = min(basso[v], basso[w])
            elif w in in_pila: basso[v] = min(basso[v], indice[w])
        if basso[v] == indice[v]:
            c = []
            while True:
                w = pila.pop(); in_pila.discard(w); c.append(w)
                if w == v: break
            if len(c) > 1 or v in grafo.get(v, []): cicli.append(sorted(c))
    for v in sorted(raggiunti):
        if v not in indice: visita(v)
    for c in cicli:
        print(os.path.basename(f), ' '.join(f'{pc:04x}:{decode(d, pc)[1]}' for pc in c))
