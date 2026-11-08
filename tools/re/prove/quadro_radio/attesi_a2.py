# I numeri di proposta.md §9.4 e §9.8 dalle righe QUADRO della corsa di base, con le regole scritte li'. Solo lettura.
# Uso: attesi_a2.py [cartella della corsa]
import sys, pathlib, collections, math
B = pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else '/home/randolph/Claude/melchior-688/engine/build_radio/base')
M18 = 'I02 I05 I06 I07 S03 S07 S08 S11 S15'.split()
P18 = [f'{m}_{s}' for m in M18 for s in (1, 2)]
ALTRI_S11 = [f'S11_{s}' for s in range(3, 9)]   # i semi in piu', solo per scegliere quello del cancello
SCENE = {'m4a': 'M4 A', 'm4b': 'M4 B', 'm4c': 'M4 C', 's5': 'S5', 's5spia': 'S5 spia'}
def righe(n):
    f = B / f'{n}.txt'
    if not f.exists(): return None
    v = []
    for l in f.read_text(errors='replace').splitlines():
        if l.startswith('QUADRO '):
            a = l.split()
            d = {'_': a[1]}
            for x in a[2:]:
                if '=' in x:
                    k, w = x.split('=', 1); d[k] = w
            v.append(d)
    return v
def conta(n):
    R = righe(n); c = collections.Counter()
    if R is None: return None
    gruppi = set(); usati = set(); virt = collections.defaultdict(set); tx = collections.defaultdict(list); a_max = collections.Counter(); occ = []
    for d in R:
        w = d['_']
        if w == 'salita':
            al = int(d['a'])
            c['Sb' if al < 2 else 'Sa'] += 1
            if al >= 2 and d['rotto'] == '1': c['Sr'] += 1
        elif w == 'copia':
            if d['ost'] == '1': gruppi.add((d['id'], int(d['t']) // 20)); c['G_a2' if int(d['a']) >= 2 else 'G_sotto'] += 0
            c['copie'] += 1
        elif w == 'usato':
            usati.add((d['id'], d['su'], float(d['t']) // 900))
        elif w == 'rapporto':
            al = int(d['a'])
            if d['antenna'] == '0': c['R20'] += 1
            else:
                c['Rb' if al < 2 else 'Ra'] += 1
                tx[(d['id'], d['lato'])].append(float(d['t']))
        elif w == 'flash':
            if d['lato'] == '16' and int(d['a']) >= 2 and float(d['quota']) <= 300 and float(d['vel']) <= 15 and (float(d['quota']) > 120 or float(d['vel']) > 12 or d['rotto'] == '1'):
                c['E'] += 1
        elif w == 'allerta':
            a_max[d['id']] = max(a_max[d['id']], int(d['a']))
        elif w == 'affondato':
            c['A' if d['giocatore'] == '1' else 'aff_ia'] += 1
            if d['giocatore'] == '0': c[f"aff_{d['id']}"] += 1
        elif w == 'occasione':
            if d['nemico'] == '1':
                c['O'] += 1; c['O_' + d['sensore']] += 1
                occ.append(d)
        elif w == 'ciclo':
            msg = [x.split(':') for x in d.get('msg', '').split(',') if x]
            nuovi = [x for x in msg if (x[0], x[1]) not in virt[d['id']]]
            lim = int(d['lim'])
            if int(d['a']) >= 2 and lim > 0 and d['rotto'] == '0' and nuovi:
                peso = 1.0 if lim == 1 else 0.5
                if any(x[2] == '1' for x in nuovi):
                    c['C'] += peso
                    if d['esposto'] == '1': c['C_gia_albero'] += peso
                    if d['libero'] == '1' and any(x[3] == '1' for x in nuovi): c['Uc'] += peso
                    if d['libero'] == '1' and d['esposto'] == '0':   # perche' il dato non serve: si scompone Uc
                        o = [x for x in nuovi if x[2] == '1']
                        if any(x[3] == '1' for x in o): pass
                        elif not any(float(x[4]) <= 30000 for x in o): c['Ux_oltre30'] += peso
                        elif int(d['a']) >= 3 and not any(float(x[4]) < 10000 for x in o): c['Ux_a3'] += peso
                        else: c['Ux_roe'] += peso
                    if d['libero'] == '1' and any(x[2] == '1' and float(x[4]) <= 30000 for x in nuovi): c['Ul'] += peso
                for x in nuovi: virt[d['id']].add((x[0], x[1]))
            if msg: c['cicli_msg'] += 1
    c['G'] = len(gruppi); c['U'] = len(usati)
    # le trasmissioni: al piu' una ogni 60 s per battello
    for (i, lato), tt in tx.items():
        ult = -1e18
        for t in sorted(tt):
            if t - ult >= 60.0:
                c['tx_urss' if lato == '16' else 'tx_usa'] += 1; ult = t
    c['a2_toccata'] = sum(1 for v in a_max.values() if v >= 2); c['battelli'] = len(a_max)
    return c, a_max, occ
def binom(n, p):
    def cdf(k):
        return sum(math.comb(n, i) * p**i * (1 - p)**(n - i) for i in range(k + 1))
    lo = 0
    while lo < n and cdf(lo) < 0.025: lo += 1
    hi = 0
    while hi < n and cdf(hi) < 0.975: hi += 1
    return lo, hi
out = {}
print('partita | Sb Sa Sr | G C (gia albero) | U Uc | Rb Ra R20 | tx urss usa | E | A aff_ia | O radar esm | battelli: allerta massima')
somma = collections.Counter()
for n in P18:
    x = conta(n)
    if x is None: print(n, 'manca'); continue
    c, am, occ = x; out[n] = (c, am, occ); somma.update({k: v for k, v in c.items()})
    print(f"{n:7} | {c['Sb']:2} {c['Sa']:3} {c['Sr']:2} | {c['G']:3} {c['C']:5.1f} ({c['C_gia_albero']:4.1f}) | {c['U']:2} {c['Uc']:4.1f} | {c['Rb']:2} {c['Ra']:3} {c['R20']:3} | {c['tx_urss']:3} {c['tx_usa']:3} | {c['E']:2} | {c['A']} {c['aff_ia']} | {c['O']:3} {c['O_radar']:3} {c['O_esm']:2} | {dict(am)}")
s = somma
print('\n== 18 partite ==')
print({k: s[k] for k in 'Sb Sa Sr G C C_gia_albero U Uc Rb Ra R20 tx_urss tx_usa E A aff_ia O O_radar O_esm'.split()})
a = lambda x: math.floor(x); b = lambda x: math.ceil(x)
print('Uc alla lettera di 9.4 (libero, ostile, entro 30 km, senza ROE ne\' limite di A3):', s['Ul'], '| senza albero, perche\' non serve: oltre 30 km', s['Ux_oltre30'], '; A3 con ellisse da 10 km', s['Ux_a3'], '; armi trattenute o altro', s['Ux_roe'])
a2 = [n for n in P18 if n[:3] not in ('S08', 'S11') and n in out]
print('Solo le partite che partono in A2: G', sum(out[n][0]['G'] for n in a2), 'C', sum(out[n][0]['C'] for n in a2), 'di cui gia\' ad albero', sum(out[n][0]['C_gia_albero'] for n in a2), '| Sa', sum(out[n][0]['Sa'] for n in a2))
print(f"Salite: P3 = {s['Sb']} + {s['Sa']}; P2+P3 fra {s['Sb']} e {b(s['Sb'] + s['Sr'] + 0.25 * s['Sa'])}")
print(f"Gruppi di copie: P3 = {s['G']}; P2+P3 almeno {a(s['G'] + 0.5 * s['C'])}")
print(f"Dato usato: P3 fra {s['U']} e {s['U'] + 1}; P2+P3 fra {s['U']} e {b(s['U'] + 1.5 * s['Uc'])}")
print(f"Rapporti: P3 fra {a(0.7 * (s['Rb'] + s['Ra']))} e {b(1.3 * (s['Rb'] + s['Ra']))}; P2+P3 fra {a(0.5 * s['Rb'])} e {b(1.3 * s['Rb'] + 0.25 * s['Ra'])}")
fu = binom(s['tx_urss'], 0.34); fa = binom(s['tx_usa'], 0.02)
print(f"Fix: URSS {s['tx_urss']} trasmissioni a 34%: {fu}; USA {s['tx_usa']} a 2% (24% di rilevamento con ESM entro 50 km): {fa}")
print(f"ELF: fra 0 e {s['E']}" + (' (zero: non si scrive)' if s['E'] == 0 else ''))
print(f"Giocatore affondato: al piu' {s['A'] + 2} (base {s['A']})")
print('\n== Scene ==')
sc = collections.defaultdict(list)
for k in SCENE:
    tot = collections.Counter(); part = 0; uc = 0; vi = 0; ric = []
    for i in range(1, 11):
        x = conta(f'{k}_{i}')
        if x is None: continue
        c, am, occ = x; part += 1; tot.update(c); uc += c['Uc'] > 0; ric.append((i, c['O'], c['Sa'], c['Ra'], c['U'], c.get('aff_12', 0), c.get('aff_6', 0)))
        out[f'{k}_{i}'] = (c, am, occ)
    print(f"{SCENE[k]:8} partite {part} | Sa {tot['Sa']} Sb {tot['Sb']} G {tot['G']} C {tot['C']} U {tot['U']} Uc {tot['Uc']} partite con Uc>0 {uc} | Ra {tot['Ra']} Rb {tot['Rb']} R20 {tot['R20']} | O {tot['O']} | A {tot['A']} affondati id12 {tot['aff_12']} id6 {tot['aff_6']}")
    print('   seme, O, Sa, Ra, U, aff12, aff6:', ric)
# il cancello
print('\n== Cancello ==')
def scegli(lista, cond):
    for n in lista:
        if n in out and cond(out[n]): return n
    return None
for n in ALTRI_S11:
    x = conta(n) if (B / f'{n}.txt').exists() else None
    if x: out[n] = x
s11 = [n for n in ('S11_1', 'S11_2', *ALTRI_S11) if n in out]
s11_a2 = scegli(s11, lambda x: x[0]['a2_toccata'] > 0)
s11_no = scegli(s11, lambda x: x[0]['a2_toccata'] == 0)
s5e = max((n for n in out if n.startswith('s5_')), key=lambda n: out[n][0]['O'], default=None)
print('S11 con A2:', s11_a2, '| S11 senza A2:', s11_no, '| S5 che si espone:', s5e, out[s5e][0]['O'] if s5e else None)
gr_b = [n for n in ['S08_1', s11_a2, s11_no] if n]
gr_a = [n for n in ['S03_1', 'S07_1', 'S07_2', 'I07_1', 'S15_1', 'm4a_1', s5e] if n]
canc = gr_a + gr_b
Ug = sum(out[n][0]['U'] for n in canc if n in out); Ucg = sum(out[n][0]['Uc'] for n in canc if n in out)
Ob = sum(out[n][0]['O'] for n in gr_b if n in out); Oa = sum(out[n][0]['O'] for n in gr_a if n in out)
print('partite:', canc)
for n in canc:
    if n in out:
        c = out[n][0]; print(f"  {n:7} U {c['U']} Uc {c['Uc']} C {c['C']} G {c['G']} Sa {c['Sa']} Sb {c['Sb']} O {c['O']} radar {c['O_radar']} esm {c['O_esm']}")
print(f"Ug {Ug} Ucg {Ucg} -> controllo 1: dati usati almeno {Ug + max(1, a(0.25 * Ucg))}" + (' | Ucg zero: non puo reggere' if Ucg == 0 else ''))
print('Ucg alla lettera:', sum(out[n][0]['Ul'] for n in canc if n in out))
def occ_tutte(n):
    return sum(1 for d in righe(n) if d['_'] == 'occasione'), sum(1 for d in righe(n) if d['_'] == 'occasione' and d['nemico'] == '0')
print('occasioni per partita (tutte, di cui di neutrali):', {n: occ_tutte(n) for n in gr_b + ['S08_2', 'S11_2'] if n in out})
print(f"Ob {Ob} -> S08 e S11 almeno {a(0.8 * Ob)}, almeno 1 in ogni partita: per partita {[(n, out[n][0]['O']) for n in gr_b if n in out]}")
print(f"Oa {Oa} -> partite che partono in A2 almeno {max(1, a(0.1 * Oa))}")
print(f"Cancello sulla carta di 9.4: C {s['C']} contro G {s['G']}: " + ('C minore di G, lo dico' if s['C'] < s['G'] else 'regge'))
