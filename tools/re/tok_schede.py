"""Schede delle missioni originali di 688 Attack Sub, estratte dal bytecode SCENE*.TOK.

Uso:  tok_schede.py <radice del progetto>

Per ogni scena esegue in modo simbolico lo script di inizializzazione (dall'offset 0 fino a END),
seguendo CALL e salti; i rami IF_RND sono esplorati entrambi con la probabilita' esatta (piu' test
consecutivi senza RANDOM usano lo stesso numero casuale: si tiene l'intervallo dei valori possibili).
Livello di gioco e lato del giocatore sono fissati ai valori predefiniti (STANDARD, giocatore = id 0).

Uscite in re/tok/schede/: <scena>.json, <scena>.md, <scena>.png (unita' e rotte sulla carta SURF).

Unita' (verificate, vedi docs/04_vm_missioni.md): 1 nm = 50 unita', 1 kt = 350, 1 ft = 8,
rotta 9000 = 360 gradi, 1 tick = 2 s, 1 pixel della carta = 32 unita'.
"""
import os, sys, json, glob, re, struct, copy
from fractions import Fraction
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from tok_ops import OPS, TERMINA
from tok_disasm import decode, carica_tabella

NM, KT, FT, GIRO, TICK_S, PIX = 50, 350, 8, 9000, 2, 32
MESSAGGI = carica_tabella('messaggi.txt')
TIPI = carica_tabella('tipi_unita.txt')
MISSIONI = {0: "HIT AND RUN", 1: "TORPEX '89", 2: "SHAKE 'EM", 3: "SUB OPS", 4: "MUMAR CADAVER",
            5: "ESCAPE", 6: "GOULASH", 7: "CAT WALK", 8: "SURPRISE PARTY", 9: "HOMECOMING"}
TEATRI = ['otranto', 'baltico', 'barents', 'gibilterra', 'kunashir', 'celtica', 'faroe']   # indice mappa A..G
# A = giocatore sul 688, B = giocatore sull'Alfa (in TORPEX: secondo 688), C = probabile partita a due
# giocatori via modem (entrambi i sottomarini in testa, script con IF_PLAYER1 = secondo giocatore)
LATO = {'A': 'lato 688', 'B': 'lato Alfa', 'C': 'due giocatori (ipotesi)'}

# --- campi del modello toccati dagli opcode ------------------------------------------------------
SEMPLICI = {0x03: 'quota', 0x04: 'p04', 0x05: 'flag', 0x06: 'rotta', 0x07: 'velocita', 0x08: 'p_quota',
            0x09: 'p_virata', 0x0a: 'p_accel', 0x0b: 'vmax', 0x0c: 'p3a', 0x0d: 'p3b', 0x0e: 'p3c',
            0x0f: 'p3d', 0x13: 'p4c', 0x28: 'tipo', 0x29: 'p3c', 0x35: 'quota_max', 0x38: 'integrita',
            0x39: 'nav', 0x46: 'uflag', 0x1e: 'comportamento', 0x21: 'telegrafo', 0x24: 'rotta_ordinata'}

def nuovo_modello():
    return {'flag': 0, 'integrita': 10, 'comportamento': None, 'arrivo': None, 'reazione': None, 'p54': 0}

def clear(m):
    for k in ('comportamento', 'arrivo', 'reazione', 'destinazione', 'telegrafo', 'nav'):   # CLEAR azzera anche +4E
        m.pop(k, None)
    m.update(flag=0, integrita=10, p54=0, comportamento=None, arrivo=None, reazione=None)

def esegui_init(d):
    """Restituisce la lista dei percorsi: dict(prob, unita, timer, var, globali, note)."""
    risultati = []
    start = dict(pc=0, stack=[], m=nuovo_modello(), unita=[], cur=None, timer=[], var={}, glob={},
                 r=(0, 255), prob=Fraction(1), note=[], passi=0)
    pila = [start]
    while pila:
        s = pila.pop()
        while True:
            s['passi'] += 1
            if s['passi'] > 20000 or len(risultati) > 100000:
                s['note'].append('percorso troncato'); risultati.append(s); break
            r = decode(d, s['pc'])
            if r is None:
                s['note'].append(f"opcode non decodificabile a {s['pc']:#06x}"); risultati.append(s); break
            op, name, args, nxt = r
            a = [v for c, v in args]
            m = s['m']; s['pc'] = nxt
            if op == 0x00:
                if s['stack']: s['pc'] = s['stack'].pop(); continue
                risultati.append(s); break
            if op == 0x01: s['stack'].append(nxt); s['pc'] = a[0]; continue
            if op == 0x43: s['pc'] = a[0]; continue
            if op in SEMPLICI: m[SEMPLICI[op]] = a[0]
            elif op == 0x02: m['x'], m['y'] = a
            elif op == 0x11: clear(m)
            elif op == 0x12: m['destinazione'] = (a[0], a[1]); m['arrivo'] = a[3]
            elif op in (0x1c, 0x1d, 0x2d): m['arrivo'], m['reazione'] = a[-2], a[-1]
            elif op == 0x3b: m['comportamento'], m['reazione'] = a[1], a[2]
            elif op == 0x2f: m['p54'] = (m['p54'] & ~2) | a[0]
            elif op == 0x30: m['p54'] = (m['p54'] & ~1) | a[0]
            elif op == 0x5c: m['p54'] = (m['p54'] & ~4) | a[0]
            elif op == 0x10:
                u = copy.deepcopy(m); u['id'] = len(s['unita']); s['unita'].append(u); s['cur'] = u['id']
            elif op == 0x15 and s['cur'] is not None:
                keep = s['unita'][s['cur']]['id']; s['unita'][s['cur']] = copy.deepcopy(m); s['unita'][s['cur']]['id'] = keep
            elif op == 0x74: s['cur'] = a[0] if a[0] < len(s['unita']) else s['cur']
            elif op == 0x14: s['timer'].append(dict(unita=s['cur'], tempo=a[0], etichetta=a[1]))
            elif op == 0x3a: s['glob']['termoclini_ft'] = [v / FT for v in a if v]
            elif op in (0x1f, 0x25, 0x26, 0x31, 0x58, 0x76): s['glob'].setdefault('gestori', {})[f'{op:#04x}'] = a
            elif op == 0x4c: s['var'][a[0]] = a[1]
            elif op == 0x4d: s['var'][a[0]] = s['var'].get(a[0], 0) + 1
            elif op == 0x51: s['var'][a[0]] = s['var'].get(a[0], 0) - 1
            elif op == 0x60: s['r'] = (0, 255)
            elif op == 0x44:          # IF_RND p, L: salta se p >= casuale
                p, lab = a; lo, hi = s['r']; n = hi - lo + 1
                sj = (lo, min(hi, p)); sf = (max(lo, p + 1), hi)
                pj = Fraction(max(0, sj[1] - sj[0] + 1), n); pf = 1 - pj
                if pj > 0:
                    t = copy.deepcopy(s); t['pc'] = lab; t['r'] = sj; t['prob'] *= pj; pila.append(t)
                if pf == 0: break
                s['r'] = sf; s['prob'] *= pf
            elif op == 0x69: s['pc'] = a[0]; s['note'].append('livello STANDARD (salto IF_STANDARD)')
            elif op == 0x68: s['note'].append('giocatore = id 0 (IF_PLAYER1 non preso)')
            elif op == 0x75: pass     # IF_TAMPERED: si assume memoria integra
            elif op in (0x4e, 0x4f, 0x50):
                v = s['var'].get(a[0], 0)
                if (op == 0x4e and v > a[1]) or (op == 0x4f and v < a[1]) or (op == 0x50 and v == a[1]):
                    s['pc'] = a[2]
            elif op == 0x48:
                if a[0] < len(s['unita']): s['pc'] = a[1]
            elif any(c in 'L' for c in OPS[op][1]) and op not in (0x14, 0x2b, 0x12, 0x1c, 0x1d, 0x2d, 0x3b, 0x1e, 0x64):
                s['note'].append(f'condizione dinamica {name} a {s["pc"] - (nxt - s["pc"]):#06x} considerata falsa')
            if op in TERMINA: break
    return risultati

# --- analisi statica degli script delle unita' --------------------------------------------------
def flusso(d, start, max_istr=400):
    """Istruzioni raggiungibili da un'etichetta (segue salti e CALL, non gli script di altre unita')."""
    vis = {}; pila = [start]
    while pila and len(vis) < max_istr:
        pc = pila.pop()
        while pc not in vis and 0 <= pc < len(d):
            r = decode(d, pc)
            if r is None: break
            vis[pc] = r; op, name, args, nxt = r
            for c, v in args:
                if c == 'L' and op not in (0x12, 0x1c, 0x1d, 0x2d, 0x3b, 0x1e, 0x64, 0x14, 0x2b):
                    pila.append(v)
            if op == 0x01: pila.append(args[0][1])
            if op in TERMINA: break
            pc = nxt
    return vis

def rotta(d, lab):
    """Segue la catena di waypoint GOTO_XY a partire dallo script all'arrivo."""
    punti = []; visti = set()
    while lab is not None and lab not in visti:
        visti.add(lab)
        prossimo = None
        for pc in sorted(flusso(d, lab, 40)):
            op, name, args, nxt = decode(d, pc)
            if op == 0x12:
                punti.append((args[0][1], args[1][1])); prossimo = args[3][1]; break
        lab = prossimo
    return punti, (lab in visti if lab is not None else False)

ETICHETTE_COMPORTAMENTO = [
    ({0x2d, 0x54, 0x6a}, 'attacca'), ({0x67, 0x66}, 'pedina (uguaglia velocita\'/quota)'),
    ({0x61}, 'accosta verso il contatto'), ({0x71, 0x63}, 'accostate evasive'),
    ({0x5d, 0x6c, 0x53}, 'controlla posizione o distanza'), ({0x57}, 'condivide i contatti'),
    ({0x12, 0x64}, 'naviga a punti'), ({0x3d, 0x3c, 0x42}, 'valuta la qualita\' della traccia'),
    ({0x55, 0x23}, 'genera messaggi'),
]

def descrivi_script(d, lab):
    if lab is None: return []
    ops = {r[0] for r in flusso(d, lab).values()}
    return [t for s, t in ETICHETTE_COMPORTAMENTO if ops & s]

def effetti_timer(d, lab):
    out = []
    for pc, (op, name, args, nxt) in sorted(flusso(d, lab).items()):
        if op in (0x55, 0x23):
            c = args[0][1]
            out.append(f'esito {c - 100}' if c >= 100 else MESSAGGI.get(c, f'msg {c:#x}'))
    return sorted(set(out))

# --- briefing e configurazione dalla scena LZA --------------------------------------------------
def briefing(root, scena):
    f = f'{root}/extracted/raw_lzw/{scena}.LZA.bin'
    if not os.path.exists(f): return {}, [], {}
    b = open(f, 'rb').read()
    righe = [m.group(1).decode('ascii').rstrip() for m in re.finditer(rb'\x1d\x00([\x20-\x7e]*)\x00', b)]
    esiti, cur = {}, None
    for r in righe:
        mm = re.match(r'CODE:\s*\d+-(\d+)-', r.strip())
        if mm: cur = int(mm.group(1)); esiti[cur] = []; continue
        if cur is not None: esiti[cur].append(r.strip())
    esiti = {k: ' '.join(v).strip() for k, v in esiti.items() if ' '.join(v).strip()}
    # pagina di configurazione: la sequenza piu' lunga di token 0B 00 <indice> <valore> consecutivi
    serie = max(re.findall(rb'(?:\x0b\x00[\x00-\x0f].){8,}', b, re.S), key=len, default=b'')
    cfg = {serie[i + 2]: serie[i + 3] for i in range(0, len(serie), 4)}
    cfg = {k: cfg[k] for k in sorted(cfg)}
    ordini = []
    for r in righe:
        if r.strip().startswith('CODE:'): break
        ordini.append(r)
    return esiti, ordini, cfg

# --- uscite ------------------------------------------------------------------------------------
def unita_leggibile(u):
    tipo = u.get('tipo')
    lato = {0x08: 'blu', 0x10: 'rosso', 0x18: 'rosso'}.get(u.get('flag', 0) & 0x18, 'neutrale')
    return dict(id=u['id'], tipo=tipo, nome=TIPI.get(tipo, '?') if tipo is not None else '?',
                flag=f"{u.get('flag', 0):#04x}", schieramento_bit=f"{u.get('flag', 0) & 0x18:#04x}",
                x_nm=round(u.get('x', 0) / NM, 1), y_nm=round(u.get('y', 0) / NM, 1),
                x=u.get('x'), y=u.get('y'),
                rotta_gradi=round(u.get('rotta', 0) * 360 / GIRO), quota_ft=round(u.get('quota', 0) / FT),
                velocita_kt=round(u.get('velocita', 0) / KT, 1),
                avanti_tutta_kt=round(u.get('vmax', 0) / KT, 1), sforzo_max_kt=round(u.get('vmax', 0) * 280 / 256 / KT, 1),
                quota_max_ft=round(u.get('quota_max', 0) / FT) if u.get('quota_max') else None,
                integrita=u.get('integrita'), sensori=[u.get(k) for k in ('p3a', 'p3b', 'p3c', 'p3d')],
                p54=u.get('p54'), comportamento=u.get('comportamento'), arrivo=u.get('arrivo'), reazione=u.get('reazione'))

def scheda(root, path):
    scena = os.path.splitext(os.path.basename(path))[0]
    d = open(path, 'rb').read()
    percorsi = esegui_init(d)
    esiti, ordini, cfg = briefing(root, scena)
    n = int(scena[5]); lato = scena[6]
    mappa = cfg.get(14)
    tot = sum(p['prob'] for p in percorsi)
    varianti = []
    for p in sorted(percorsi, key=lambda p: -p['prob']):
        un = []
        for u in p['unita']:
            ul = unita_leggibile(u)
            punti, ciclica = rotta(d, u.get('arrivo'))
            if u.get('destinazione'):
                punti = [tuple(u['destinazione'])] + punti
            ul['rotta_waypoint_nm'] = [(round(x / NM, 1), round(y / NM, 1)) for x, y in punti]
            ul['rotta_ciclica'] = ciclica
            ul['comportamento_descrizione'] = descrivi_script(d, u.get('comportamento'))
            ul['reazione_descrizione'] = descrivi_script(d, u.get('reazione'))
            ul['_punti'] = punti
            un.append(ul)
        timer = [dict(unita=t['unita'], dopo_s=(t['tempo'] & 0x7fff) * TICK_S, assoluto=bool(t['tempo'] & 0x8000),
                      etichetta=f"{t['etichetta']:#06x}", effetti=effetti_timer(d, t['etichetta'])) for t in p['timer']]
        varianti.append(dict(probabilita=float(p['prob']), probabilita_frazione=str(p['prob']), unita=un,
                             timer=timer, variabili=p['var'], globali=p['glob'], note=sorted(set(p['note']))))
    codici = set()
    from tok_disasm import disasm
    code, *_ = disasm(d)
    for pc, (op, name, args, nxt) in code.items():
        if op in (0x55, 0x23) and args[0][1] >= 100: codici.add(args[0][1] - 100)
    marg = {}
    for w in varianti:
        for u in w['unita']:
            k = (u['x_nm'], u['y_nm'], u['rotta_gradi'], u['quota_ft'])
            marg.setdefault(u['id'], {}).setdefault(k, 0.0)
            marg[u['id']][k] += w['probabilita']
    marginali = {str(i): [dict(pos_nm=[k[0], k[1]], rotta_gradi=k[2], quota_ft=k[3], probabilita=round(p, 4))
                          for k, p in sorted(v.items(), key=lambda kv: -kv[1])] for i, v in sorted(marg.items())}
    presenza = {}
    rappresentante = {}
    for w in varianti:
        for u in w['unita']:
            presenza[u['id']] = presenza.get(u['id'], 0.0) + w['probabilita']
            rappresentante.setdefault(u['id'], u)        # dalla variante piu' probabile che la contiene
    unita_tutte = []
    for i in sorted(rappresentante):
        u = dict(rappresentante[i]); u['presenza'] = round(presenza[i], 4); unita_tutte.append(u)
    n_var = len(varianti)
    varianti = varianti[:50]          # nel JSON solo le 50 combinazioni piu' probabili
    out = dict(scena=scena, missione=n or 10, nome=MISSIONI[n], lato=LATO.get(lato, lato),
               mappa=('SURF' + 'ABCDEFG'[mappa]) if mappa is not None and mappa < 7 else None,
               teatro=TEATRI[mappa] if mappa is not None and mappa < 7 else None,
               configurazione_lza={str(k): v for k, v in cfg.items()},
               somma_probabilita=float(tot), numero_varianti=n_var, posizioni_marginali=marginali,
               unita=unita_tutte,
               varianti=varianti,
               esiti={str(k): esiti.get(k, '') for k in sorted(codici | set(esiti))},
               esiti_dallo_script=sorted(codici), ordini=ordini)
    return out

def png(root, sc, dst):
    from PIL import Image, ImageDraw
    if not sc['mappa']: return
    base = Image.open(f"{root}/extracted/png/{sc['mappa']}_originale.png").convert('RGB')
    img = base.resize((base.width * 3, base.height * 3), Image.NEAREST)
    dr = ImageDraw.Draw(img)
    col = {'blu': (40, 120, 255), 'rosso': (230, 40, 40), 'neutrale': (250, 250, 250)}
    def px(x, y): return (x / PIX * 3, (199 - y / PIX) * 3)
    for u in sc['unita']:
        if u['x'] is None: continue
        c = col['blu'] if u['schieramento_bit'] == '0x08' else col['rosso'] if u['schieramento_bit'] in ('0x10', '0x18') else col['neutrale']
        pts = [px(u['x'], u['y'])] + [px(x, y) for x, y in u['_punti']]
        if len(pts) > 1: dr.line(pts, fill=c, width=2)
        x0, y0 = pts[0]
        dr.ellipse((x0 - 5, y0 - 5, x0 + 5, y0 + 5), outline=(0, 0, 0), fill=c)
        dr.text((x0 + 7, y0 - 6), str(u['id']), fill=(255, 255, 0))
    # legenda
    h = 14 * len(sc['unita']) + 22
    dr.rectangle((4, img.height - h - 4, 330, img.height - 4), fill=(0, 0, 0))
    dr.text((10, img.height - h), f"{sc['scena']} {sc['nome']} ({sc['lato']})  blu: flag 0x08  rosso: 0x10", fill=(255, 255, 255))
    for k, u in enumerate(sc['unita']):
        dr.text((10, img.height - h + 14 * (k + 1)), f"{u['id']}: {u['nome'][:34]}" + (f"  ({u['presenza']:.0%})" if u['presenza'] < 1 else ''), fill=(255, 255, 0))
    img.save(dst)

def md(sc):
    v = sc['varianti'][0]
    r = [f"# {sc['scena']}: missione {sc['missione']}, {sc['nome']} ({sc['lato']})", '',
         f"Teatro: **{sc['teatro']}** (mappa {sc['mappa']}). Combinazioni iniziali: {sc['numero_varianti']}; "
         "per le unita' con piu' posizioni possibili la tabella mostra quella della combinazione piu' probabile che le contiene.", '']
    if v['globali'].get('termoclini_ft'): r += [f"Termoclini: {', '.join(f'{t:.0f} ft' for t in v['globali']['termoclini_ft'])}.", '']
    r += ['| Id | Unita\' | Presenza | Pos. (nm) | Rotta | Quota | Vel. | Max (avanti/sforzo) | Quota max | Rotta a punti | Comportamento |',
          '|---|---|---|---|---|---|---|---|---|---|---|']
    for u in sc['unita']:
        wp = ' → '.join(f'({x}, {y})' for x, y in u['rotta_waypoint_nm'][:6]) + (' ↺' if u['rotta_ciclica'] else '')
        r.append(f"| {u['id']} | {u['nome']} | {u['presenza']:.0%} | ({u['x_nm']}, {u['y_nm']}) | {u['rotta_gradi']}° | {u['quota_ft']} ft | "
                 f"{u['velocita_kt']} kt | {u['avanti_tutta_kt']} / {u['sforzo_max_kt']} kt | "
                 f"{str(u['quota_max_ft']) + ' ft' if u['quota_max_ft'] else '—'} | {wp or '—'} | "
                 f"{', '.join(u['comportamento_descrizione']) or '—'} |")
    multi = {i: v for i, v in sc['posizioni_marginali'].items() if len(v) > 1}
    if multi:
        r += ['', f"**Posizioni iniziali casuali** ({sc['numero_varianti']} combinazioni in tutto)", '']
        for i, pos in multi.items():
            r.append(f"- unita' {i}: " + '; '.join(f"({m['pos_nm'][0]}, {m['pos_nm'][1]}) {m['rotta_gradi']}° {m['quota_ft']} ft → {m['probabilita']:.0%}" for m in pos))
    if v['timer']:
        r += ['', '**Timer all\'avvio**', '', '| Unita\' | Dopo | Effetti |', '|---|---|---|']
        for t in v['timer']:
            r.append(f"| {t['unita']} | {t['dopo_s']} s{' (ass.)' if t['assoluto'] else ''} | {', '.join(t['effetti']) or '—'} |")
    if sc['esiti']:
        r += ['', '**Esiti di fine missione**', '']
        for k, txt in sc['esiti'].items():
            fonte = 'script' if int(k) in sc['esiti_dallo_script'] else 'motore'
            r.append(f"- **{k}** ({fonte}): {txt[:300] or '(nessun testo)'}")
    notes = v['note']
    if notes: r += ['', 'Note: ' + '; '.join(notes)]
    return '\n'.join(r) + '\n'

if __name__ == '__main__':
    root = sys.argv[1]
    dst = f'{root}/re/tok/schede'; os.makedirs(dst, exist_ok=True)
    indice = []
    for path in sorted(glob.glob(f'{root}/extracted/raw/DISK1/SCENE*.TOK')):
        sc = scheda(root, path)
        for w in sc['varianti']:
            for u in w['unita']: u.pop('_punti', None)
        for u in sc['unita']: u.pop('_punti', None)
        json.dump(sc, open(f"{dst}/{sc['scena']}.json", 'w'), indent=1, ensure_ascii=False)
        # rigenera i punti per l'immagine (rimossi dal JSON)
        sc2 = scheda(root, path); png(root, sc2, f"{dst}/{sc['scena']}.png")
        open(f"{dst}/{sc['scena']}.md", 'w').write(md(sc))
        v = sc['varianti'][0]
        indice.append(dict(scena=sc['scena'], missione=sc['missione'], nome=sc['nome'], lato=sc['lato'],
                           teatro=sc['teatro'], unita=len(v['unita']), combinazioni=sc['numero_varianti'],
                           esiti=sc['esiti_dallo_script']))
        print(f"{sc['scena']}: m{sc['missione']:<2} {sc['nome']:<15} {sc['lato']:<9} {str(sc['teatro']):<10} unita' {len(v['unita']):2d} "
              f"varianti {sc['numero_varianti']:5d} somma p {sc['somma_probabilita']:.4f} esiti {sc['esiti_dallo_script']} note {v['note'][:2]}")
    json.dump(indice, open(f'{dst}/indice.json', 'w'), indent=1, ensure_ascii=False)

def indice_md(root):
    """Tabella riassuntiva di tutte le scene (re/tok/schede/INDICE.md)."""
    from collections import Counter
    righe = ['| Scena | Missione | Lato | Teatro | Forze (unita\' create all\'avvio, presenza < 100% tra parentesi) | Combinazioni | Esiti nello script |',
             '|---|---|---|---|---|---|---|']
    for f in sorted(glob.glob(f'{root}/re/tok/schede/SCENE*.json')):
        sc = json.load(open(f))
        c = Counter(); parz = []
        for u in sc['unita']:
            nome = u['nome'].replace(' CLASS', '').replace('ATTACK SUBMARINE', 'SSN').title()
            if u['presenza'] < 0.999: parz.append(f"{nome} ({u['presenza']:.0%})")
            else: c[nome] += 1
        forze = ', '.join(f'{n}× {k}' if n > 1 else k for k, n in c.items()) + (('; ' + ', '.join(parz)) if parz else '')
        righe.append(f"| [{sc['scena']}](../re/tok/schede/{sc['scena']}.md) | {sc['missione']} {sc['nome']} | {sc['lato']} | {sc['teatro']} | "
                     f"{forze} | {sc['numero_varianti']} | {', '.join(map(str, sc['esiti_dallo_script']))} |")
    open(f'{root}/re/tok/schede/INDICE.md', 'w').write('\n'.join(righe) + '\n')
    return '\n'.join(righe)

if __name__ == '__main__':
    indice_md(sys.argv[1])
