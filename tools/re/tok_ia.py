"""Comportamenti delle unita' nell'originale: gli script di comportamento e di reazione delle 22 scene.

Ogni unita' ha tre script (docs/04_vm_missioni.md): all'arrivo (+0E), di comportamento periodico (+10) e di
reazione (+56). Vengono assegnati all'inizio (schede re/tok/schede) e poi cambiati dagli script stessi con
BEHAVIOR, ENGAGE, PURSUE, PURSUE2, LAUNCH, GOTO_XY e GOTO_TGT. Lo strumento trova tutte le etichette usate
come comportamento o reazione, ne segue il flusso (tok_schede.flusso) e scrive lo pseudo-codice con le
descrizioni della tabella degli opcode, insieme alle classi che li hanno all'inizio della scena.

Uso:  tok_ia.py <radice>  ->  re/tok/ia/<SCENA>.txt e re/tok/ia/statistiche.json
"""
import os, sys, glob, json, collections
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import tok_schede as T
from tok_disasm import decode
from tok_ops import OPS, TERMINA

CONDIZIONI = {n for op, (n, a, dsc) in OPS.items() if n.startswith('IF_')}
AZIONI = {'TURN_TO_TGT', 'TURN_AWAY', 'TURN_REL', 'TURN_TO', 'HEAD_TO', 'MATCH_SPEED', 'MATCH_DEPTH', 'TELEGRAPH',
          'DEPTH_TO', 'GOTO_XY', 'GOTO_TGT', 'PURSUE', 'PURSUE2', 'ENGAGE', 'LAUNCH', 'ATTACK', 'ATTACK_RND', 'F54_1',
          'F54_2', 'F54_4', 'STOP_NAV', 'BEHAVIOR', 'NO_BEHAVIOR', 'TARGET_ID', 'RETARGET', 'SPEED_STEP', 'PING', 'MISSILE', 'TRANSIENT'}

def ruoli(d):
    """Etichetta -> insieme dei ruoli, da tutto il codice raggiungibile dall'inizio e dai gestori."""
    r = collections.defaultdict(set); visti = set(); pila = [0]
    while pila:
        p = pila.pop()
        while p not in visti and 0 <= p < len(d):
            visti.add(p); x = decode(d, p)
            if x is None: break
            op, name, args, nxt = x; lab = [v for c, v in args if c == 'L']
            if name == 'BEHAVIOR': r[lab[0]].add('comportamento')
            elif name == 'ENGAGE': r[lab[0]].add('comportamento'); r[lab[1]].add('reazione')
            elif name in ('PURSUE', 'PURSUE2', 'LAUNCH'): r[lab[0]].add('arrivo'); r[lab[1]].add('reazione')
            elif name in ('GOTO_XY', 'GOTO_TGT'): r[lab[-1]].add('arrivo')
            elif name == 'AT': r[lab[0]].add('timer')
            pila += lab
            if name.startswith('HANDLERS'): pila += [v for c, v in args]
            if op in TERMINA: break
            p = nxt
    return r

def pseudo(d, lab):
    righe = []; ops = []
    for pc, (op, name, args, nxt) in sorted(T.flusso(d, lab, 80).items()):
        a = ', '.join(f'L_{v:04x}' if c == 'L' else str(v) for c, v in args)
        righe.append(f'  {pc:04x}  {name:14s} {a:28s} ; {OPS[op][2]}'); ops.append(name)
    return righe, ops

if __name__ == '__main__':
    root = sys.argv[1]; dst = f'{root}/re/tok/ia'; os.makedirs(dst, exist_ok=True)
    stat = collections.defaultdict(lambda: collections.Counter()); per_classe = collections.defaultdict(collections.Counter)
    for f in sorted(glob.glob(f'{root}/extracted/raw/DISK1/SCENE*.TOK')):
        sc = os.path.basename(f)[:7]; d = open(f, 'rb').read(); r = ruoli(d)
        scheda = json.load(open(f'{root}/re/tok/schede/{sc}.json'))
        iniziali = collections.defaultdict(set)
        for u in scheda['unita']:
            for k in ('comportamento', 'reazione', 'arrivo'):
                if u.get(k) is not None: iniziali[u[k]].add(u['nome'])
        out = [f'# {sc}: script di comportamento e di reazione', '']
        for lab in sorted(l for l, v in r.items() if v & {'comportamento', 'reazione'}):
            righe, ops = pseudo(d, lab)
            classi = sorted(iniziali.get(lab, ()))
            out += [f"== L_{lab:04x}  [{', '.join(sorted(r[lab]))}]" + (f"  classi all'inizio: {', '.join(classi)}" if classi else '')] + righe + ['']
            for ruolo in r[lab] & {'comportamento', 'reazione'}:
                stat[ruolo].update(o for o in set(ops) if o in CONDIZIONI | AZIONI)
            for c in classi: per_classe[c].update(o for o in set(ops) if o in CONDIZIONI | AZIONI)
        open(f'{dst}/{sc}.txt', 'w').write('\n'.join(out) + '\n')
    json.dump(dict(per_ruolo={k: dict(v.most_common()) for k, v in stat.items()},
                   per_classe={k: dict(v.most_common()) for k, v in per_classe.items()}),
              open(f'{dst}/statistiche.json', 'w'), indent=1, ensure_ascii=False)
    for k, v in stat.items(): print(k, v.most_common(25))
    for c, v in sorted(per_classe.items()): print(f'{c[:34]:34s}', v.most_common(8))
